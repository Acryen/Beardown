// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameState.h"

#include "Net/UnrealNetwork.h"

void AFlipperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperGameState, PlayerOneScore);
	DOREPLIFETIME(AFlipperGameState, PlayerTwoScore);
}
