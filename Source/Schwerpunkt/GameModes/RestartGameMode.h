// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RestartGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API ARestartGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandleOne;
	void CheckForPlayerNumber();

	/* Should there be more than one minigame in the future, the ServerTravel Path could be made a public variable that would be changed by
	 * the RestartMatch function to the appropriate map and then use the variable here. */
	
};
