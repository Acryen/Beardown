// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "Schwerpunkt/Character/FlipperGameCharacter.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers >= MaxNumberOfPlayersInLobby)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(PathToGameMap);
		}
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
		if (SchwerpunktGameInstanceSubsystem)
		{
			MaxNumberOfPlayersInLobby = SchwerpunktGameInstanceSubsystem->SessionMaxPlayers;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%i"), MaxNumberOfPlayersInLobby));
}

void ALobbyGameMode::KillPlayerByKillZ(APawn* PlayerPawn)
{
	APlayerController* FlipperPlayerController = Cast<APlayerController>(PlayerPawn->GetController());
	AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(PlayerPawn);
	if (!FlipperPlayerController || !FlipperGameCharacter) return;

	FlipperGameCharacter->DestroyWeapon();
	FlipperGameCharacter->Reset();
	RestartPlayer(FlipperPlayerController);
}
