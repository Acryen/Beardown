// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SchwerpunktGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API USchwerpunktGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	int32 SessionMaxPlayers = 2;
	int32 RequiredGoalsToWin = 2;
	bool bAlreadyJoinedSession = false;
};
