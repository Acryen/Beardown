// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "StartingMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API AStartingMapGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
