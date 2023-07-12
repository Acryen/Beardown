// Fill out your copyright notice in the Description page of Project Settings.


#include "StartingMapGameMode.h"

#include "Schwerpunkt/GameInstances/MultiplayerGameInstanceSubsystem.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"

void AStartingMapGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;
	
	USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
	if (SchwerpunktGameInstanceSubsystem && SchwerpunktGameInstanceSubsystem->bAlreadyJoinedSession)
	{
		UMultiplayerGameInstanceSubsystem* MultiplayerGameInstanceSubsystem = GameInstance->GetSubsystem<UMultiplayerGameInstanceSubsystem>();
		if (!MultiplayerGameInstanceSubsystem) return;

		MultiplayerGameInstanceSubsystem->DestroySession();
		SchwerpunktGameInstanceSubsystem->bAlreadyJoinedSession = false;

		// TODO Debug Message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("DestroySession in StartingMapGameMode wurde aufgerufen"));
	}
}
