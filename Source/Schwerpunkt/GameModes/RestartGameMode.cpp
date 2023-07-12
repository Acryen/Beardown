// Fill out your copyright notice in the Description page of Project Settings.


#include "RestartGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"

void ARestartGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &ThisClass::CheckForPlayerNumber, 1.f, true);
}

void ARestartGameMode::CheckForPlayerNumber()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;
	USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
	if (!SchwerpunktGameInstanceSubsystem) return;
	
	TArray<AActor*> PlayerArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), DefaultPawnClass, PlayerArray);

	if (PlayerArray.Num() == SchwerpunktGameInstanceSubsystem->SessionMaxPlayers)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleOne);
		
		GetWorld()->ServerTravel(FString(TEXT("/Game/Maps/TimMap2?listen")));
	}
}
		
