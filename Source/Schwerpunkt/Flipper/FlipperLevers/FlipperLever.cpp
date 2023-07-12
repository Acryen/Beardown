

#include "FlipperLever.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Schwerpunkt/Schwerpunkt.h"
#include "Schwerpunkt/Flipper/FlipperArms/FlipperArm.h"
#include "Schwerpunkt/GameModes/FlipperGameMode.h"

AFlipperLever::AFlipperLever()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	SetRootComponent(LeverMesh);
	LeverMesh->SetGenerateOverlapEvents(true);
	LeverMesh->SetCollisionObjectType(ECC_Lever);
	LeverMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	LeverMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	HighlightingLaserBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightingLaserBeam"));
	HighlightingLaserBeam->SetupAttachment(GetRootComponent());
	HighlightingLaserBeam->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	if (HighlightingLaserBeamMaterial)
	{
		HighlightingLaserBeam->SetMaterial(0, HighlightingLaserBeamMaterial);
	}

	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolvTimelineComponent"));
}

void AFlipperLever::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperLever, MovementState);
}

void AFlipperLever::BeginPlay()
{
	Super::BeginPlay();

	SetupMaterialInstances();

	// Set the Highlighting LaserBeam to the correct position and rotation
	if (!CorrespondingArm) return;
	FVector TargetDirection = (CorrespondingArm->GetActorLocation() - (GetActorLocation() + FVector(0, 0, 125))).GetSafeNormal();
	HighlightingLaserBeam->SetWorldLocation(GetActorLocation() + FVector(0, 0, 125) + TargetDirection * HighlightingLaserBeamDistance);
	HighlightingLaserBeam->SetWorldRotation(TargetDirection.Rotation());
	HighlightingLaserBeam->SetVisibility(false);
	
	// For raising and lowering the lever.
	StartLocation = GetActorLocation();
	TargetLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z - MovingDownDistance);

	// Pass reference from itself to GameMode and automatically to StageManager as well.
	AFlipperGameMode* FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	
	if (ActorHasTag(FName(TEXT("StartingLever"))))
	{
		if (FlipperGameMode)
		{
			FlipperGameMode->AddStartingLever(this);
		}
	}
	else if (ActorHasTag(FName(TEXT("StageOneLever"))))
	{
		if (FlipperGameMode)
		{
			FlipperGameMode->AddStageOneFlipperLever(this);
		}
	}
	else if (ActorHasTag(FName(TEXT("StageTwoLever"))))
	{
		if (FlipperGameMode)
		{
			FlipperGameMode->AddStageTwoFlipperLever(this);
		}

		// Because StageTwoLevers start down, StartLocation and TargetLocation have to be swapped once.
		FVector NewStartLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + MovingDownDistance);
		StartLocation = NewStartLocation;
		TargetLocation = GetActorLocation();		
	}
	else if (ActorHasTag(FName(TEXT("KeeperLever"))))
	{
		if (FlipperGameMode)
		{
			FlipperGameMode->AddKeeperFlipperLever(this);
		}
	}
}

void AFlipperLever::SetupMaterialInstances()
{
	// For highlighting the lever when the player is standing in front of it.
	ButtonGlowDynamicMaterialInstance = UMaterialInstanceDynamic::Create(LeverMesh->GetMaterial(2), this);
	if (!ButtonGlowDynamicMaterialInstance) return;
	LeverMesh->SetMaterial(2, ButtonGlowDynamicMaterialInstance);

	// For dissolving the mesh. One Dynamic Material for each of the three slots.
	WoodDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(LeverMesh->GetMaterial(0), this);
	MetalDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(LeverMesh->GetMaterial(1), this);
	ButtonDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(ButtonDissolveMaterialInstance, this);
}

void AFlipperLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MovementState)
	{
	case EMovementState::EMS_NoMovement:
		break;
		
	case EMovementState::EMS_MoveDown:
		MoveDown(DeltaTime);
		break;
		
	case EMovementState::EMS_MoveUp:
		MoveUp(DeltaTime);
		break;

	default:
		break;
	}
}

void AFlipperLever::ReactToHit(AActor* HitInstigator)
{
	// Return when we're ramping up/down or we don't have an arm to trigger.
	if (MovementState != EMovementState::EMS_NoMovement) return;
	if (!CorrespondingArm)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s hat keinen CorrespondingArm ausgewählt"), *GetName()));
		return;
	}

	// Actual functionality
	CorrespondingArm->ActivateArm();

	MulticastPlayHitSound();

	// If the lever should be a starter lever, it should go down after use.
	if (bMoveDownAfterUse)
	{
		MovementState = EMovementState::EMS_MoveDown;

		// The levers and arms, which are only there for starters, should disappear after the first use of a lever.
		if (ActorHasTag(FName(TEXT("StartingLever"))))
		{
			TArray<AActor*> StartingTools;
			UGameplayStatics::GetAllActorsWithTag(this, FName(TEXT("StartingLever")), StartingTools);
			for (AActor* Actor : StartingTools)
			{
				if (Actor != this)
				{
					AFlipperLever* OtherStartingLever = Cast<AFlipperLever>(Actor);
					if (OtherStartingLever)
					{
						OtherStartingLever->MoveDownOtherStartingLever();
					}
				}
			}
		}
	}

	// The lever should not be able to be hit more than once in one attack.
	HitBlacklistArray.Add(HitInstigator);
	FTimerHandle TimerHandleOne;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &ThisClass::RemoveFromHitBlacklist, 1);
}

void AFlipperLever::RemoveFromHitBlacklist()
{
	if (HitBlacklistArray.Num() == 0) return;
	
	HitBlacklistArray.RemoveAt(0);
}

void AFlipperLever::MulticastPlayHitSound_Implementation()
{
	if (GettingHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GettingHitSound, GetActorLocation(), GetActorRotation());
	}
}

void AFlipperLever::MoveDown(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MovingDownSpeed));

	if (GetActorLocation().Equals(TargetLocation, 1))
	{
		MulticastDeactivateStartingTriggerBox();		
		MovementState = EMovementState::EMS_NoMovement;
	}
}

// This function is called from the other lever. With the other lever, this function sets bMovingDownActive to true, which then causes it to move down in its Tick() function.
void AFlipperLever::MoveDownOtherStartingLever()
{
	MovementState = EMovementState::EMS_MoveDown;

	AFlipperArm* CorrespondingFlipperArm = Cast<AFlipperArm>(CorrespondingArm);
	if (CorrespondingFlipperArm)
	{
		CorrespondingFlipperArm->ActivateMovingDown();
	}
}

void AFlipperLever::MulticastDeactivateStartingTriggerBox_Implementation()
{
	// Disable Collision from the TriggerBox, which keeps the ball straight at the beginning.
	if (StartingTriggerBox)
	{
		StartingTriggerBox->SetActorEnableCollision(false);
	}
}

void AFlipperLever::MoveUp(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), StartLocation, DeltaTime, MovingDownSpeed));

	if (GetActorLocation().Equals(StartLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
	}
}

// This function is called by FlipperGoal's overlap event (the event only exists on the server).
void AFlipperLever::MoveUpWithCorrespondingArm()
{
	MovementState = EMovementState::EMS_MoveUp;
	MulticastActivateStartingTriggerBox();

	AFlipperArm* CorrespondingFlipperArm = Cast<AFlipperArm>(CorrespondingArm);
	if (CorrespondingFlipperArm)
	{
		CorrespondingFlipperArm->ActivateMovingUp();
	}
}

void AFlipperLever::MulticastActivateStartingTriggerBox_Implementation()
{
	if (StartingTriggerBox)
	{
		StartingTriggerBox->SetActorEnableCollision(true);
	}
}

void AFlipperLever::SetEnableHighlightingBeam(bool bVisibleStatus)
{
	if (!HighlightingLaserBeam) return;
	HighlightingLaserBeam->SetVisibility(bVisibleStatus);
}

void AFlipperLever::SetEnableGlow(bool bGlowStatus)
{	
	if (bGlowStatus)
	{
		ButtonGlowDynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 30.f);
		CorrespondingArm->SetEnableGlow(true);
	}
	else
	{
		ButtonGlowDynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 1.f);
		CorrespondingArm->SetEnableGlow(false);
	}
}

void AFlipperLever::StartDissolve()
{
	if (CorrespondingArm)
	{
		CorrespondingArm->StartDissolve();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperLever::DisableCastShadows, CastShadowDelay);
	
	LeverMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (!WoodDissolveDynamicMaterialInstance || !MetalDissolveDynamicMaterialInstance || !ButtonDissolveDynamicMaterialInstance) return;
	LeverMesh->SetMaterial(0, WoodDissolveDynamicMaterialInstance);
	LeverMesh->SetMaterial(1, MetalDissolveDynamicMaterialInstance);
	LeverMesh->SetMaterial(2, ButtonDissolveDynamicMaterialInstance);

	DissolveTrackDelegate.BindDynamic(this, &AFlipperLever::UpdateDissolveMaterial);

	if (DissolveCurve && DissolveTimelineComponent)
	{
		DissolveTimelineComponent->AddInterpFloat(DissolveCurve, DissolveTrackDelegate);
		DissolveTimelineComponent->Play();
	}
}

void AFlipperLever::UpdateDissolveMaterial(float DissolveValue)
{
	if (!WoodDissolveDynamicMaterialInstance || !MetalDissolveDynamicMaterialInstance || !ButtonDissolveDynamicMaterialInstance) return;
	WoodDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	MetalDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	ButtonDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
}

void AFlipperLever::DisableCastShadows()
{
	LeverMesh->SetCastShadow(false);
}