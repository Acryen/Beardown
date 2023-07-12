// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Schwerpunkt/Schwerpunkt.h"
#include "Schwerpunkt/Flipper/FlipperLevers/FlipperLever.h"
#include "Schwerpunkt/Flipper/FlipperObstacles/FlipperObstacleBase.h"
#include "Schwerpunkt/Miscellaneous/StageManager.h"


AFlipperGameCharacter::AFlipperGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	TriggerBoxForLever = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBoxForLever"));
	TriggerBoxForLever->SetupAttachment(RootComponent);
	TriggerBoxForLever->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerBoxForLever->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ExclamationMarkMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExclamationMarkMesh"));
	ExclamationMarkMesh->SetupAttachment(RootComponent);
	ExclamationMarkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExclamationMarkMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void AFlipperGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperGameCharacter, Weapon);
	DOREPLIFETIME(AFlipperGameCharacter, bIsStunned);
}

void AFlipperGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	if (WeaponBaseBlueprint)
	{
		Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponBaseBlueprint);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_r"));
		Weapon->SetOwner(this);
		Weapon->SetFlipperGameCharacter(this);
	}

	TriggerBoxForLever->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBoxForLever->SetCollisionResponseToChannel(ECC_Lever, ECollisionResponse::ECR_Overlap);
	TriggerBoxForLever->OnComponentBeginOverlap.AddDynamic(this, &AFlipperGameCharacter::OnTriggerBoxForLeverBeginOverlap);
	TriggerBoxForLever->OnComponentEndOverlap.AddDynamic(this, &AFlipperGameCharacter::OnTriggerBoxForLeverEndOverlap);
}

void AFlipperGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::PerformJump);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AFlipperGameCharacter::Attack);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFlipperGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFlipperGameCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
}

void AFlipperGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsStunned)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFlipperGameCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && !bIsStunned)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFlipperGameCharacter::PerformJump()
{
	if (!bIsStunned)
	{
		Jump();
	}
}

void AFlipperGameCharacter::EnablePlayerMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->bOrientRotationToMovement = true; 
}

void AFlipperGameCharacter::MulticastEnablePlayerMovement_Implementation()
{
	EnablePlayerMovement();
}

void AFlipperGameCharacter::ServerDisablePlayerMovement_Implementation()
{
	MulticastDisablePlayerMovement();
}

void AFlipperGameCharacter::MulticastDisablePlayerMovement_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AFlipperGameCharacter::MulticastDisablePhysicsInteraction_Implementation()
{
	GetCharacterMovement()->bEnablePhysicsInteraction = false;
}

void AFlipperGameCharacter::Attack()
{
	if (!bIsStunned)
	{
		ServerAttack();
	}
}

void AFlipperGameCharacter::ServerAttack_Implementation()
{
	MulticastAttack();
}

void AFlipperGameCharacter::MulticastAttack_Implementation()
{
	if (AttackMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

// Die function wird nur auf dem Server aufgerufen und soll aktuell den Spieler ein Stück weit zurückschubsen.
void AFlipperGameCharacter::ReactToHit(AActor* HitInstigator)
{
	if (HitBlacklistArray.Contains(HitInstigator)) return;
	
	FVector InitialTargetDirection = (GetActorLocation() - HitInstigator->GetActorLocation()).GetSafeNormal() * CharacterLaunchVelocity;
	FVector TargetDirection = FVector(InitialTargetDirection.X, InitialTargetDirection.Y, InitialTargetDirection.Z + CharacterLaunchZCorrection);
	GetCharacterMovement()->AddImpulse(TargetDirection, true);

	// Der Spieler soll pro gegnerischen Angriff nicht mehr als ein mal getroffen werden können.
	HitBlacklistArray.Add(HitInstigator);
	FTimerHandle TimerHandleOne;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &ThisClass::RemoveFromHitBlacklist, 1);

	MulticastPlayStunMontage();
	MulticastPlayHitSound();	
	
	// if (!bIsStunned)
	// 	PerformStun();
}

void AFlipperGameCharacter::RemoveFromHitBlacklist()
{
	if (HitBlacklistArray.Num() == 0) return;
	
	HitBlacklistArray.RemoveAt(0);
}

void AFlipperGameCharacter::MulticastPlayStunMontage_Implementation()
{
	if (StunnedMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(StunnedMontage);
		FTimerHandle TimerHandleTwo;
		GetWorld()->GetTimerManager().SetTimer(TimerHandleTwo, this, &ThisClass::ReleaseStun, AnimaDuration);
		// For the sake of simplicity, ReleaseStun() is called because without calling PerformStun() first, the function just canceled the animation.
	}
}

void AFlipperGameCharacter::MulticastPlayHitSound_Implementation()
{
	if (GettingHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GettingHitSound, GetActorLocation(), GetActorRotation());
	}
}

// This function used to be called in ReactToHit and is currently not used anymore.
void AFlipperGameCharacter::PerformStun()
{
	bIsStunned = true;
	if (StunnedMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(StunnedMontage);
	}
	
	FTimerHandle StunnedTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StunnedTimerHandle, this, &AFlipperGameCharacter::ReleaseStun, StunDuration);
}

void AFlipperGameCharacter::ReleaseStun()
{
	if (StunnedMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, StunnedMontage);
	}
		
	bIsStunned = false;
}

// Consequently, this function is also currently not used
void AFlipperGameCharacter::OnRep_bIsStunned()
{
	if (!StunnedMontage) return;
	if (bIsStunned)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(StunnedMontage);
	}
	else
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, StunnedMontage);
	}
}

void AFlipperGameCharacter::SetupClothingMaterials(bool bIsRedPlayer)
{
	if (bIsRedPlayer) return;
	
	GetMesh()->SetMaterial(5, ClothingMaterialInstance);
}

void AFlipperGameCharacter::ServerSetupClothingMaterials_Implementation(bool IsRedPlayer)
{
	MulticastSetupClothingMaterials(IsRedPlayer);
}

// This function is called by the FlipperPlayerController in BeginPlay
void AFlipperGameCharacter::MulticastSetupClothingMaterials_Implementation(bool bIsRedPlayer)
{	
	// The default material is red. Only if the player is on the blue team we want to change the material.
	if (bIsRedPlayer) return;
	
	GetMesh()->SetMaterial(5, ClothingMaterialInstance);
}

void AFlipperGameCharacter::DestroyWeapon()
{
	if (Weapon)
	{
		Weapon->Destroy();
	}
}

void AFlipperGameCharacter::OnTriggerBoxForLeverBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	AFlipperLever* FlipperLever = Cast<AFlipperLever>(OtherActor);
	if (!FlipperLever) return;
	if (IsLocallyControlled()) // Friendly reminder: The overlap event is only caused on the server.
	{
		FlipperLever->SetEnableGlow(true);
		FlipperLever->SetEnableHighlightingBeam(true);
	}
	else
	{
		ClientSetEnableGlow(FlipperLever, true);
		ClientSetEnableHighlightingBeam(FlipperLever, true);
	}
}

void AFlipperGameCharacter::OnTriggerBoxForLeverEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	AFlipperLever* FlipperLever = Cast<AFlipperLever>(OtherActor);
	if (!FlipperLever) return;
	if (IsLocallyControlled())
	{
		FlipperLever->SetEnableGlow(false);
		FlipperLever->SetEnableHighlightingBeam(false);
	}
	else
	{
		ClientSetEnableGlow(FlipperLever, false);
		ClientSetEnableHighlightingBeam(FlipperLever, false);
	}
}

void AFlipperGameCharacter::ClientSetEnableHighlightingBeam_Implementation(AFlipperLever* FlipperLever,
	bool bVisibilityStatus)
{
	if (!FlipperLever) return;
	FlipperLever->SetEnableHighlightingBeam(bVisibilityStatus);
}

void AFlipperGameCharacter::ClientSetEnableGlow_Implementation(AFlipperLever* FlipperLever, bool bGlowStatus)
{
	if (!FlipperLever) return;
	FlipperLever->SetEnableGlow(bGlowStatus);
}

void AFlipperGameCharacter::MulticastEnableExclamation_Implementation()
{
	if (ExclamationMarkSound)
	{
		UGameplayStatics::PlaySound2D(this, ExclamationMarkSound);
	}
	
	ExclamationTimelineDelegate.BindDynamic(this, &AFlipperGameCharacter::UpdateZScale);

	if (TimelineComponent && ZScaleCurve)
	{
		TimelineComponent->AddInterpFloat(ZScaleCurve, ExclamationTimelineDelegate);
		TimelineComponent->Play();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperGameCharacter::HideExclamationMark, ExclamationMarkDuration);
}

void AFlipperGameCharacter::UpdateZScale(float TimelineValue)
{
	if (!ExclamationMarkMesh) return;
	ExclamationMarkMesh->SetWorldScale3D(FVector(0.9, 0.9, TimelineValue));
}

void AFlipperGameCharacter::HideExclamationMark()
{
	if (!ExclamationMarkMesh) return;
	ExclamationMarkMesh->SetVisibility(false);
}

/*
 * TODO DEBUG FUNCTION
 */

void AFlipperGameCharacter::InitiateStageThree()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Initiate Stage Three"));

	TArray<AActor*> StageManagerArray;
	UGameplayStatics::GetAllActorsOfClass(this, AStageManager::StaticClass(), StageManagerArray);
	
	if (StageManagerArray.IsEmpty()) return;
	AStageManager* StageManager = Cast<AStageManager>(StageManagerArray[0]);
	
	TArray<AActor*> ActorArray;
	TArray<AFlipperLever*> FlipperLeverArray;
	UGameplayStatics::GetAllActorsWithTag(this, FName(FString(TEXT("StartingLever"))), ActorArray);
	for (AActor* Actor : ActorArray)
	{
		FlipperLeverArray.Add(Cast<AFlipperLever>(Actor));
	}
	
	TArray<AActor*> OtherActorArray;
	TArray<AFlipperLever*> OtherFlipperArray;
	UGameplayStatics::GetAllActorsWithTag(this, FName(FString(TEXT("StageTwoLever"))), OtherActorArray);
	for (AActor* Actor : OtherActorArray)
	{
		OtherFlipperArray.Add(Cast<AFlipperLever>(Actor));
	}
	
	TArray<AActor*> AnotherActorArray;
	TArray<AFlipperObstacleBase*> ObstacleArray;
	UGameplayStatics::GetAllActorsOfClass(this, AFlipperObstacleBase::StaticClass(), AnotherActorArray);
	for (AActor* Actor : AnotherActorArray)
	{
		ObstacleArray.Add(Cast<AFlipperObstacleBase>(Actor));
	}
	
	if (StageManager)
	{
		StageManager->ActivateStageThree(0.5);
	}
}
