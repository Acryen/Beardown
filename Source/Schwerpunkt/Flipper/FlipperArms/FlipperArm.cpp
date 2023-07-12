// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperArm.h"

#include "Kismet/GameplayStatics.h"


AFlipperArm::AFlipperArm()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
		
	// Mesh is inherited from FlipperArmBase.h
	Mesh->SetupAttachment(GetRootComponent());
}

void AFlipperArm::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetActorRotation();
	TargetRotation = FRotator(StartRotation.Pitch, StartRotation.Yaw, StartRotation.Roll - RotationAngle);

	StartLocation = GetActorLocation();
	TargetLocation = FVector(StartLocation.X + MovingDownDistance, StartLocation.Y, StartLocation.Z);
}

void AFlipperArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MovementState)
	{
	case EMovementState::EMS_NoMovement:
		break;
		
	case EMovementState::EMS_RotateForward:
		RotateArmForward(DeltaTime);
		break;
		
	case EMovementState::EMS_RotateBackward:
		RotateArmBackward(DeltaTime);
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

void AFlipperArm::RotateArmForward(float DeltaTime)
{
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeedForward));
	
	if (GetActorRotation().Equals(TargetRotation, RotationTargetEqualTolerance))
	{
		MovementState = EMovementState::EMS_RotateBackward;
	}
}

void AFlipperArm::RotateArmBackward(float DeltaTime)
{
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), StartRotation, DeltaTime, RotationSpeedBackward));

	if (GetActorRotation().Equals(StartRotation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;

		if (bMoveDownAfterUse)
		{
			MovementState = EMovementState::EMS_MoveDown;
		}
	}
}

void AFlipperArm::ActivateArm()
{
	Super::ActivateArm();

	MovementState = EMovementState::EMS_RotateForward;

	MulticastPlayRotateSound();
}

void AFlipperArm::MulticastPlayRotateSound_Implementation()
{
	if (RotateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RotateSound, GetActorLocation(), GetActorRotation());
	}
}

void AFlipperArm::MoveDown(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MovingDownSpeed));

	if (GetActorLocation().Equals(TargetLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
	}
}

void AFlipperArm::MoveUp(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), StartLocation, DeltaTime, MovingDownSpeed));

	if (GetActorLocation().Equals(StartLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
	}
}
