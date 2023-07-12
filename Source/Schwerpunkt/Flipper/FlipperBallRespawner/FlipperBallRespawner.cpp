// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBallRespawner.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Schwerpunkt/Flipper/FlipperBall/FlipperBall.h"
#include "Schwerpunkt/Flipper/FlipperLevers/FlipperLever.h"

AFlipperBallRespawner::AFlipperBallRespawner()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
}

void AFlipperBallRespawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFlipperBallRespawner::OnTriggerBoxOverlap);

	// Set StartingLever References to bring them back up on respawn.
	TArray<AActor*> StartingLeverArray;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("StartingLever")), StartingLeverArray);
	if (StartingLeverArray.Num() < 2) return;
	StartingLeverOne = Cast<AFlipperLever>(StartingLeverArray[0]);
	StartingLeverTwo = Cast<AFlipperLever>(StartingLeverArray[1]);
}

void AFlipperBallRespawner::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FlipperBall = Cast<AFlipperBall>(OtherActor);
	if (!FlipperBall) return;
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperBallRespawner::RestartFlipperBall, FlipperBallResetTime);
}

void AFlipperBallRespawner::RestartFlipperBall()
{
	// bGameOver may be modified by the ChildClass FlipperGoal.
	if (bGameOver || !FlipperBall || !FlipperBallSpawnPointActor) return;
	FlipperBall->SetActorLocation(FlipperBallSpawnPointActor->GetActorLocation());

	// Raise the starting lever and arm again.
	StartingLeverOne->MoveUpWithCorrespondingArm();
	StartingLeverTwo->MoveUpWithCorrespondingArm();
}