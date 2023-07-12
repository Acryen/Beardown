// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperArmObstacle.h"


AFlipperArmObstacle::AFlipperArmObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(GetRootComponent());
}

void AFlipperArmObstacle::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetActorRotation();
	TargetRotation = FRotator(StartRotation.Pitch, StartRotation.Yaw, StartRotation.Roll - RotationAngle);

	if (bStartActive)
	{
		MovementState = EMovementState::EMS_RotateForward;
	}
}

void AFlipperArmObstacle::Tick(float DeltaTime)
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

	default:
		break;
	}
}

void AFlipperArmObstacle::RotateArmForward(float DeltaTime)
{
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeedForward));

	if (HasAuthority() && GetActorRotation().Equals(TargetRotation, 1))
	{
		MovementState = EMovementState::EMS_RotateBackward;
	}
}

void AFlipperArmObstacle::RotateArmBackward(float DeltaTime)
{
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), StartRotation, DeltaTime, RotationSpeedForward));

	if (HasAuthority() && GetActorRotation().Equals(StartRotation, 1))
	{
		MovementState = EMovementState::EMS_RotateForward;
	}
}