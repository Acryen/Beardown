// Fill out your copyright notice in the Description page of Project Settings.


#include "KillZActor.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Schwerpunkt/GameModes/FlipperGameMode.h"
#include "Schwerpunkt/GameModes/LobbyGameMode.h"

AKillZActor::AKillZActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

}

void AKillZActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKillZActor::OnTriggerBoxOverlap);
	}	
}

void AKillZActor::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn) return;

	AFlipperGameMode* FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	if (FlipperGameMode)
	{
		FlipperGameMode->KillPlayerByKillZ(PlayerPawn);
		return;
	}

	ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->KillPlayerByKillZ(PlayerPawn);
	}
}
