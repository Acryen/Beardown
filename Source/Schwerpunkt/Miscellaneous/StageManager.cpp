// Fill out your copyright notice in the Description page of Project Settings.


#include "StageManager.h"

#include "KillZActor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Schwerpunkt/Schwerpunkt.h"
#include "Schwerpunkt/Character/FlipperGameCharacter.h"
#include "Schwerpunkt/Controllers/FlipperPlayerController.h"
#include "Schwerpunkt/Flipper/FlipperArms/FlipperArmBase.h"
#include "Schwerpunkt/Flipper/FlipperBall/FlipperBall.h"
#include "Schwerpunkt/Flipper/FlipperBall/FlipperBallSpawnPoint.h"
#include "Schwerpunkt/Flipper/FlipperLevers/FlipperLever.h"
#include "Schwerpunkt/Flipper/FlipperObstacles/FlipperObstacleBase.h"
#include "Schwerpunkt/GameModes/FlipperGameMode.h"
#include "Sound/AmbientSound.h"


AStageManager::AStageManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
}

void AStageManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStageManager, BoardTransitionState);
	DOREPLIFETIME(AStageManager, StartingLeverArray);
	DOREPLIFETIME(AStageManager, StageTwoLeverArray);
	DOREPLIFETIME(AStageManager, KeeperLeverArray);
	DOREPLIFETIME(AStageManager, bRedTeamHasWon);
}

void AStageManager::BeginPlay()
{
	Super::BeginPlay();

	FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	if (FlipperGameMode)
	{
		FlipperGameMode->SetStageManager(this);
	}
	
	// Get referances to all stage one geometry
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StageOneActor")), StageOneGeometryArray);

	// Setz die TargetLocation für die StageOneActors in das StageOneActorsTargetLocationArray.
	for (AActor* Actor : StageOneGeometryArray)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		FVector TargetLocation(ActorLocation.X + FlipperGeometryMovingDistance, ActorLocation.Y, ActorLocation.Z);
		StageOneGeometryTargetLocationArray.Add(TargetLocation);
	}
	
	// Get references to all stage two geometry
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StageTwoActor")), StageTwoGeometryArray);

	for (AActor* Actor : StageTwoGeometryArray)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		FVector TargetLocation(ActorLocation.X - FlipperGeometryMovingDistance, ActorLocation.Y, ActorLocation.Z);
		StageTwoGeometryTargetLocationArray.Add(TargetLocation);
	}

	// Get references to all stage three geometry
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StageThreeActor")), StageThreeActorArray);

	for (AActor* Actor : StageThreeActorArray)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		FVector TargetLocation(ActorLocation.X, ActorLocation.Y, ActorLocation.Z + GroundGeometryMovingDistance);
		StageThreeGeometryTargetLocationArray.Add(TargetLocation);
	}

	// Get references to all starting stage geometry
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StartingGeometry")), StartingStageGeometryArray);

	for (AActor* Actor : StartingStageGeometryArray)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		FVector TargetLocation(ActorLocation.X, ActorLocation.Y, ActorLocation.Z - GroundGeometryMovingDistance);
		StartingStageGeometryTargetLocationArray.Add(TargetLocation);
	}

	// Get references to stage three boarder collision boxes
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StageThreeCollision")), StageThreeCollisionBoxesArray);

	for (AActor* Actor : StageThreeCollisionBoxesArray)
	{
		Actor->SetActorEnableCollision(false);
	}
}

void AStageManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (BoardTransitionState)
	{
	case EBoardTransitionState::EBTS_NoTransition:
		break;
		
	case EBoardTransitionState::EBTS_PullStageOne:
		PullStageOneBack(DeltaTime);
		break;
		
	case EBoardTransitionState::EBTS_PushStageTwo:
		PushStageTwo(DeltaTime);
		break;
		
	case EBoardTransitionState::EBTS_PullStartingStage:
		PullStartingStage(DeltaTime);
		break;
		
	case EBoardTransitionState::EBTS_PushStageThree:
		PushStageThree(DeltaTime);
		break;
		
	default:
		break;
	}
}

bool AStageManager::MoveStageGeometry(float DeltaSeconds, TArray<AActor*>& GeometryArray,
	TArray<FVector>& TargetLocationArray, float MovingSpeed)
{
	int32 ActorMovementsDone = 0;
	
	for (int32 i = 0; i < GeometryArray.Num(); i++)
	{
		// Interp Location of all StageActors 
		GeometryArray[i]->SetActorLocation(FMath::VInterpConstantTo(
			GeometryArray[i]->GetActorLocation(),
			TargetLocationArray[i],
			DeltaSeconds,
			MovingSpeed));

		// For each actor that has reached its goal, we increase ActorMovementsDone
		if (GeometryArray[i]->GetActorLocation().Equals(TargetLocationArray[i], 1.f))
		{
			ActorMovementsDone++;
		}
		
		// And should ActorMovementsDone reach the count of all our StageActors, we're done!
		if (ActorMovementsDone == GeometryArray.Num())
		{
			return true;	
		}
	}
	return false;
}

// This function is called by GameMode and therefore only on the server.
void AStageManager::ActivateStageTwo(TArray<class AFlipperLever*> StartingFlipperLeverArray, TArray<class AFlipperLever*> StageOneFlipperLeverArray,
	TArray<class AFlipperLever*> StageTwoFlipperLeverArray,	TArray<class AFlipperLever*> KeeperFlipperLeverArray)
{
	StartingLeverArray = StartingFlipperLeverArray;
	StageOneLeverArray = StageOneFlipperLeverArray;
	StageTwoLeverArray = StageTwoFlipperLeverArray;
	KeeperLeverArray = KeeperFlipperLeverArray;
	BoardTransitionState = EBoardTransitionState::EBTS_PullStageOne;

	MulticastPlayPullStageOneSound();
}

void AStageManager::MulticastPlayPullStageOneSound_Implementation()
{
	if (PullStageOneSound && FlipperBoard)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PullStageOneSound,
			FlipperBoard->GetActorLocation(),
			FlipperBoard->GetActorRotation());
	}
}

// Diese function wird in Tick() auferufen
void AStageManager::PullStageOneBack(float DeltaSeconds)
{
	// MoveStageGeometry returned true, sobald alle Actors ihre TargetLocation erreicht haben.
	if(MoveStageGeometry(DeltaSeconds, StageOneGeometryArray, StageOneGeometryTargetLocationArray, FlipperGeometryMoveSpeed))
	{
		BoardTransitionState = EBoardTransitionState::EBTS_PushStageTwo;
		PlayPushStageTwoSound();
	}
		
	
	if (bMoveDownStageOneLeversNotCalled)
	{
		MoveDownStageOneLevers();
		bMoveDownStageOneLeversNotCalled = false;
	}
}

void AStageManager::PlayPushStageTwoSound()
{
	if (PushStageTwoSound && FlipperBoard)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PushStageTwoSound,
			FlipperBoard->GetActorLocation(),
			FlipperBoard->GetActorRotation());
	}
}

void AStageManager::MoveDownStageOneLevers()
{
	for (AFlipperLever* FlipperLever : StageOneLeverArray)
	{
		FlipperLever->ActivateMovingDown();
	}
}

// Diese function wird in Tick() aufgerufen
void AStageManager::PushStageTwo(float DeltaSeconds)
{	
	if (MoveStageGeometry(DeltaSeconds, StageTwoGeometryArray, StageTwoGeometryTargetLocationArray, FlipperGeometryMoveSpeed))
	{
		BoardTransitionState = EBoardTransitionState::EBTS_NoTransition;

		for (AFlipperObstacleBase* FlipperObstacle : StageTwoObstacleArray)
		{
			FlipperObstacle->StartMovement();
		}
	}

	if (bMoveUpStageTwoLeversNotCalled)
	{
		MoveUpStageTwoLevers();
		bMoveUpStageTwoLeversNotCalled = false;
	}
}

void AStageManager::MoveUpStageTwoLevers()
{
	for (AFlipperLever* FlipperLever : StageTwoLeverArray)
	{
		FlipperLever->ActivateMovingUp();
	}
}

// Diese function wird vom GameMode aufgerufen und somit automatisch auch nur auf dem Server
void AStageManager::ActivateStageThree(float StartingDelay)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	FTimerHandle TimerHandle;
	TimerManager.SetTimer(TimerHandle, FlipperBall, &AFlipperBall::MulticastStartDissolve, StartingDelay);

	FTimerHandle TimerHandleFour;
	TimerManager.SetTimer(TimerHandleFour, this, &AStageManager::StopObstacleMovement, StartingDelay);

	FTimerHandle TimerHandleThirteen;
	TimerManager.SetTimer(TimerHandleThirteen, this, &AStageManager::MulticastPlayStageTwoBrokenSound, StartingDelay);

	FTimerHandle TimerhandleTwo;
	float Delay = StartingDelay + StartingLeverDissolveDelay;
	TimerManager.SetTimer(TimerhandleTwo, this, &AStageManager::MulticastDissolveStartingLevers, Delay);

	FTimerHandle TimerHandleThree;
	Delay += StageTwoLeverDissolveDelay;
	TimerManager.SetTimer(TimerHandleThree, this, &AStageManager::MulticastDissolveStageTwoLevers, Delay);

	FTimerHandle TimerHandleFourteen;
	Delay += EarthquakeDelay;
	TimerManager.SetTimer(TimerHandleFourteen, this, &AStageManager::MulticastPlayEarthquakeSound, Delay);

	FTimerHandle TimerHandleFive;
	Delay += CameraShakeDelay;
	TimerManager.SetTimer(TimerHandleFive, this, &AStageManager::StartCameraShake, Delay);

	FTimerHandle TimerHandleSix;
	Delay += ExclamationDelay;
	TimerManager.SetTimer(TimerHandleSix, this, &AStageManager::ShowExclamationMark, Delay);

	FTimerHandle TimerHandleSeven;
	Delay += DisableMovementDelay;
	TimerManager.SetTimer(TimerHandleSeven, this, &AStageManager::DisablePlayerMovement, Delay);

	FTimerHandle TimerHandleEight;
	Delay += CameraStartBlendDelay;
	TimerManager.SetTimer(TimerHandleEight, this, &AStageManager::StartCameraBlend, Delay);

	// Delay + 2.f wird hier gehardcoded und entspricht der CameraBlendDuration vom Start zum Ziel.
	FTimerHandle TimerhandleTen;
	TimerManager.SetTimer(TimerhandleTen, this, &AStageManager::StartStageThreeGeometryTransition, Delay + 2.f);

	FTimerHandle TimerHandleTwelve;
	Delay += ReverseBallDissolveDelay;
	TimerManager.SetTimer(TimerHandleTwelve, this, &AStageManager::ReverseBallDissolve, Delay);

	FTimerHandle TimerHandleNine;
	Delay += CameraStopBlendDelay;
	TimerManager.SetTimer(TimerHandleNine, this, &AStageManager::StopCameraBlend, Delay);

	FTimerHandle TimerhandleEleven;
	TimerManager.SetTimer(TimerhandleEleven, this, &AStageManager::EnablePlayerMovement, Delay + 2.f);
}

void AStageManager::StopObstacleMovement()
{
	for (AFlipperObstacleBase* FlipperObstacle : StageTwoObstacleArray)
	{
		FlipperObstacle->MulticastStopMovement();
	}
}

void AStageManager::MulticastPlayStageTwoBrokenSound_Implementation()
{
	if (StageTwoBrokenSound && StartingLeverArray.Num() > 0 && StartingLeverArray[0]->CorrespondingArm)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			StageTwoBrokenSound,
			StartingLeverArray[0]->CorrespondingArm->GetActorLocation(),
			StartingLeverArray[0]->CorrespondingArm->GetActorRotation());
	}
}

void AStageManager::MulticastDissolveStartingLevers_Implementation()
{
	for (AFlipperLever* FlipperLever : StartingLeverArray)
	{
		FlipperLever->StartDissolve();
	}
}

void AStageManager::MulticastDissolveStageTwoLevers_Implementation()
{
	for (AFlipperLever* FlipperLever : KeeperLeverArray)
	{
		StageTwoLeverArray.Add(FlipperLever);
	}
	
	for (AFlipperLever* FlipperLever : StageTwoLeverArray)
	{
		FlipperLever->StartDissolve();
	}
}

void AStageManager::MulticastPlayEarthquakeSound_Implementation()
{
	if (EarthquakeSound)
	{
		UGameplayStatics::PlaySound2D(this, EarthquakeSound);
	}
}

void AStageManager::StartCameraShake()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		PlayerController->MulticastStartCameraShake();
	}
}

void AStageManager::ShowExclamationMark()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(PlayerController->GetPawn());
		if (FlipperGameCharacter)
		{
			FlipperGameCharacter->MulticastEnableExclamation();
		}
	}
}

void AStageManager::DisablePlayerMovement()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(PlayerController->GetPawn());
		if (FlipperGameCharacter)
		{
			FlipperGameCharacter->MulticastDisablePlayerMovement();
		}
	}
}

void AStageManager::StartCameraBlend()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		PlayerController->MulticastStartCameraBlend();
	}
}

void AStageManager::StartStageThreeGeometryTransition()
{
	BoardTransitionState = EBoardTransitionState::EBTS_PullStartingStage;

	EnableBoarderCollisionBoxes();
	MulticastPlayStartingStageTransitionSound();
}

void AStageManager::MulticastPlayStartingStageTransitionSound_Implementation()
{
	if (StartingStageTransitionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, StartingStageTransitionSound, GetActorLocation(), GetActorRotation());
	}
}

void AStageManager::EnableBoarderCollisionBoxes()
{
	for (AActor* Actor : StageThreeCollisionBoxesArray)
	{
		Actor->SetActorEnableCollision(true);
	}
}

void AStageManager::StopCameraBlend()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		PlayerController->MulticastStopCameraBlend();
	}
}

void AStageManager::EnablePlayerMovement()
{
	if (!FlipperGameMode) return;

	for (AFlipperPlayerController* PlayerController : FlipperGameMode->GetAllPlayerControllers())
	{
		AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(PlayerController->GetPawn());
		if (FlipperGameCharacter)
		{
			FlipperGameCharacter->MulticastEnablePlayerMovement();
		}
	}
}

// Diese function wird in Tick() aufgerufen
void AStageManager::PullStartingStage(float DeltaSeconds)
{
	if (MoveStageGeometry(DeltaSeconds, StartingStageGeometryArray, StartingStageGeometryTargetLocationArray, GroundGeometryMovingSpeed))
	{
		BoardTransitionState = EBoardTransitionState::EBTS_PushStageThree;
	}
}

// Diese function wird in Tick() aufgerufen
void AStageManager::PushStageThree(float DeltaSeconds)
{
	if (MoveStageGeometry(DeltaSeconds, StageThreeActorArray, StageThreeGeometryTargetLocationArray, GroundGeometryMovingSpeed))
	{
		BoardTransitionState = EBoardTransitionState::EBTS_NoTransition;

		for (AFlipperObstacleBase* FlipperObstacle : StageThreeObstacleArray)
		{
			FlipperObstacle->StartMovement();
		}
	}
}

// Setz den FlipperBall Spawnpoint auf die Position des StageManagers
void AStageManager::ReverseBallDissolve()
{
	FlipperBall->SetActorLocation(GetActorLocation());
	FlipperBall->MulticastReverseDissolve();

	// Positioniere nicht nur den FlipperBall um, sondern nimm den RespawnPoint auch gleich mit.
	TArray<AActor*> FlipperBallSpawnPointArray;
	UGameplayStatics::GetAllActorsOfClass(this, AFlipperBallSpawnPoint::StaticClass(), FlipperBallSpawnPointArray);
	if (FlipperBallSpawnPointArray.IsEmpty()) return;
	FlipperBallSpawnPointArray[0]->SetActorLocation(GetActorLocation());
}

void AStageManager::MulticastStageTwoToEndTransition_Implementation()
{
	TArray<AActor*> KillZActorArray;
	UGameplayStatics::GetAllActorsOfClass(this, AKillZActor::StaticClass(), KillZActorArray);
	for (AActor* Actor : KillZActorArray)
	{
		Actor->Destroy();
	}
	
	for (AFlipperLever* FlipperLever : KeeperLeverArray)
	{
		StageTwoLeverArray.Add(FlipperLever);
	}

	for (AFlipperLever* FlipperLever : StartingLeverArray)
	{
		StageTwoLeverArray.Add(FlipperLever);
	}
	
	for (auto Actor : StageTwoLeverArray)
	{
		Actor->Destroy();
	}

	EnablePodiumAndCage();
	PlayVictoryMusic();
}

void AStageManager::EnablePodiumAndCage()
{
	if (!WinnerPodium) return;
	WinnerPodium->GetStaticMeshComponent()->SetVisibility(true);
	WinnerPodium->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WinnerPodium->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	if (!bRedTeamHasWon && PodiumBlueMaterial)
	{
		WinnerPodium->GetStaticMeshComponent()->SetMaterial(3, PodiumBlueMaterial);
	}
	
	if (!Cage) return;
	Cage->GetStaticMeshComponent()->SetVisibility(true);
	Cage->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Cage->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Cage->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Cage->GetStaticMeshComponent()->SetEnableGravity(true);
	Cage->GetStaticMeshComponent()->SetSimulatePhysics(true);

	if (HasAuthority())
	{
		Cage->SetReplicates(true);
		Cage->SetReplicateMovement(true);
		// Cage->bStaticMeshReplicateMovement = true;
	}

	if (!EndOfGameFlipperLever) return;
	EndOfGameFlipperLever->GetMesh()->SetVisibility(true);
	EndOfGameFlipperLever->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EndOfGameFlipperLever->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	EndOfGameFlipperLever->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	if (!EndOfGameFlipperArm) return;
	EndOfGameFlipperArm->GetMesh()->SetVisibility(true);
	EndOfGameFlipperArm->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EndOfGameFlipperArm->GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	EndOfGameFlipperArm->GetMesh()->SetCollisionResponseToChannel(ECC_Lever, ECollisionResponse::ECR_Ignore);

	if (!FlipperBall) return;
	FlipperBall->Destroy();
}

void AStageManager::PlayVictoryMusic()
{
	if (!AmbientSoundActor) return;
	AmbientSoundActor->AdjustVolume(1.f, 0.f);

	if (!VictoryMusic) return;
	UGameplayStatics::PlaySound2D(this, VictoryMusic);
}

void AStageManager::MulticastStageThreeToEndTransition_Implementation()
{
	TArray<AActor*> KillZActorArray;
	UGameplayStatics::GetAllActorsOfClass(this, AKillZActor::StaticClass(), KillZActorArray);
	for (AActor* Actor : KillZActorArray)
	{
		Actor->Destroy();
	}
	
	for (auto Actor : StageThreeActorArray)
	{
		Actor->Destroy();
	}
	
	for (auto Actor : StartingStageGeometryArray)
	{
		FVector CurrentLocation = Actor->GetActorLocation();
		FVector TargetLocation = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z + GroundGeometryMovingDistance);
		Actor->SetActorLocation(TargetLocation);
	}

	EnablePodiumAndCage();
	PlayVictoryMusic();
}
