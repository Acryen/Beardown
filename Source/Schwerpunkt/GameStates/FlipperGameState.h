// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FlipperGameState.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API AFlipperGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/*
	 * Player Scores
	 */
	
	UPROPERTY(Replicated)
	int32 PlayerOneScore = 0;
	UPROPERTY(Replicated)
	int32 PlayerTwoScore = 0;

public:
	FORCEINLINE void IncreaseScore(bool bIsPlayerOne) { bIsPlayerOne ? PlayerOneScore++ : PlayerTwoScore++; }
	FORCEINLINE int32 GetPlayerOneScore() { return PlayerOneScore; }
	FORCEINLINE int32 GetPlayerTwoScore() { return PlayerTwoScore; }
 };
