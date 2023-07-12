// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperSpinnerObstacle.h"

AFlipperSpinnerObstacle::AFlipperSpinnerObstacle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	if (!Mesh) return;
	Mesh->SetupAttachment(GetRootComponent());
}

void AFlipperSpinnerObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	if (bStartActive)
	{
		StartMovement();
	}
}

void AFlipperSpinnerObstacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch(MovementState)
	{
	case EMovementState::EMS_NoMovement:
		break;
		
	case EMovementState::EMS_RotateForward:
		RotateForward(DeltaSeconds);
		break;

	default:
		break;
	}
}

void AFlipperSpinnerObstacle::RotateForward(float DeltaTime)
{
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));

	if (GetActorRotation().Equals(TargetRotation, 1))
	{
		StartRotation = GetActorRotation();
		UpdateTargetRotation(bRotateClockwise);
	}
}

void AFlipperSpinnerObstacle::UpdateTargetRotation(bool bDoesRotateClockwise)
{
	// +/-90 is hardcoded because the obstacle is supposed to rotate indefinitely here anyway and 90 degrees is just an intermediate goal.

	if (bDoesRotateClockwise)
	{
		TargetRotation = FRotator(StartRotation.Pitch, StartRotation.Yaw + 90.f, StartRotation.Roll);
	}
	else
	{
		TargetRotation = FRotator(StartRotation.Pitch, StartRotation.Yaw - 90.f, StartRotation.Roll);
	}
}

void AFlipperSpinnerObstacle::StartMovement()
{
	Super::StartMovement();

	StartRotation = GetActorRotation();
	UpdateTargetRotation(bRotateClockwise);
	MovementState = EMovementState::EMS_RotateForward;
}
