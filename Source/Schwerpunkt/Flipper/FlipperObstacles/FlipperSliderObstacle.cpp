// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperSliderObstacle.h"


AFlipperSliderObstacle::AFlipperSliderObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	MovementTarget = CreateDefaultSubobject<USceneComponent>("MovementTarget");
	MovementTarget->SetupAttachment(RootComponent);
}

void AFlipperSliderObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (bStartActive)
	{
		StartMovement();
	}
}

void AFlipperSliderObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (MovementState)
	{
	case EMovementState::EMS_NoMovement:
		break;
		
	case EMovementState::EMS_MoveForward:
		MoveForward(DeltaTime);
		break;
		
	case EMovementState::EMS_MoveBackward:
		MoveBackward(DeltaTime);
		break;

	default:
		break;
	}
}

void AFlipperSliderObstacle::MoveForward(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MoveSpeed));

	if (HasAuthority() && GetActorLocation().Equals(TargetLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
			
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperObstacleBase::ActivateMoveBackward, MovePauseDuration);
	}
}

void AFlipperSliderObstacle::MoveBackward(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), StartLocation, DeltaTime, MoveSpeed));

	if (HasAuthority() && GetActorLocation().Equals(StartLocation, 1))
	{
		MovementState = EMovementState::EMS_NoMovement;
			
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperObstacleBase::ActivateMoveForward, MovePauseDuration);
	}
}

void AFlipperSliderObstacle::StartMovement()
{
	Super::StartMovement();

	StartLocation = GetActorLocation();
	TargetLocation = MovementTarget->GetComponentLocation();

	MovementState = EMovementState::EMS_MoveForward;
}
