// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBallSpeedIncrease.h"

#include "FlipperBall.h"
#include "Components/BoxComponent.h"


AFlipperBallSpeedIncrease::AFlipperBallSpeedIncrease()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	SetRootComponent(TriggerBox);
}

void AFlipperBallSpeedIncrease::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFlipperBallSpeedIncrease::OnTriggerBoxBeginOverlap);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFlipperBallSpeedIncrease::OnTriggerBoxEndOverlap);
	}
}

void AFlipperBallSpeedIncrease::OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFlipperBall* FlipperBall = Cast<AFlipperBall>(OtherActor);
	if (FlipperBall)
	{
		FlipperBall->SetSpeed(SpeedIncrease);
	}
}

void AFlipperBallSpeedIncrease::OnTriggerBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFlipperBall* FlipperBall = Cast<AFlipperBall>(OtherActor);
	if (FlipperBall)
	{
		FlipperBall->SetSpeed(1.f);
	}
}
