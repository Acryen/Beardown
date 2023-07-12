// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMessage.h"

void ULobbyMessage::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	MenuTearDown();
}

void ULobbyMessage::MenuTearDown()
{
	RemoveFromParent();
}
