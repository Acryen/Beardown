// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void KillPlayerByKillZ(APawn* PlayerPawn);
	
	int32 MaxNumberOfPlayersInLobby = 2;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	FString PathToGameMap = FString(TEXT("/Game/Maps/TimMap2?listen"));
};
