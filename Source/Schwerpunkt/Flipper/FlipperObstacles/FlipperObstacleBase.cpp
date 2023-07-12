// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperObstacleBase.h"

#include "Net/UnrealNetwork.h"
#include "Schwerpunkt/Miscellaneous/StageManager.h"


AFlipperObstacleBase::AFlipperObstacleBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* Even if the clients move the obstacles themselves and the enum (which specifies the direction of movement) is replicated,
	 * the client obstacles can lose their sync if the client tabs out of the game. Therefore, the movement is also replicated.
	 * In contrast to the idea of only letting the server do the movement and then replicating the movement, the new solution does not stutter on the clients either.
	 * The obstacle is only changed to the correct position with a short jerk if the client tabs out. */
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
}

void AFlipperObstacleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperObstacleBase, MovementState);
}


void AFlipperObstacleBase::BeginPlay()
{
	Super::BeginPlay();

	// Set a reference to itself in the StageManager.
	if (!StageManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s hat keinen StageManager gesetzt"), *GetName()));
		return;
	}

	if (bIsStageTwoObstacle)
	{
		StageManager->AddStageTwoObstacleToArray(this);
	}
	else
	{
		StageManager->AddStageThreeObstacleToArray(this);
	}
}

void AFlipperObstacleBase::MulticastStopMovement_Implementation()
{
	MovementState = EMovementState::EMS_NoMovement;

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

