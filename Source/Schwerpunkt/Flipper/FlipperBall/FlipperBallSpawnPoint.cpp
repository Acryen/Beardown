// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBallSpawnPoint.h"


#include "Kismet/GameplayStatics.h"
#include "Schwerpunkt/Flipper/FlipperBallRespawner/FlipperBallRespawner.h"


AFlipperBallSpawnPoint::AFlipperBallSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AFlipperBallSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	TArray<AActor*> FlipperGoalArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlipperBallRespawner::StaticClass(), FlipperGoalArray);
	for (AActor* Actor : FlipperGoalArray)
	{
		AFlipperBallRespawner* FlipperGoal = Cast<AFlipperBallRespawner>(Actor);
		if (!FlipperGoal) return;
			FlipperGoal->SetFlipperBallSpawnActor(this);
	}
}