// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBlockade.h"

#include "Kismet/GameplayStatics.h"


AFlipperBlockade::AFlipperBlockade()
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void AFlipperBlockade::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	if (bMoveForwardInsteadOfLeft)
	{
		TargetLocation = StartLocation + (GetActorForwardVector() * MovingDistance);
	}
	else
	{
		TargetLocation = StartLocation - (GetActorRightVector() * MovingDistance);
	}
}

void AFlipperBlockade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MovementState)
	{
	case EMovementState::EMS_NoMovement:
		break;
		// We now equate RotateForward with MoveForward here
		
	case EMovementState::EMS_RotateForward:
		MoveForward(DeltaTime);
		break;
		
	case EMovementState::EMS_RotateBackward:
		MoveBackward(DeltaTime);
		break;

	default:
		break;
	}
}

void AFlipperBlockade::MoveForward(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MovingSpeedForward));

	if (GetActorLocation().Equals(TargetLocation, 1))
	{
		MovementState = EMovementState::EMS_RotateBackward;
	}
	
	if (MovingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MovingSound, GetActorLocation(), GetActorRotation());
	}
}

void AFlipperBlockade::MoveBackward(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), StartLocation, DeltaTime, MovingSpeedBackward));

	if (GetActorLocation().Equals(StartLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
	}
}
