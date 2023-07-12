// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerControllerBase.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API ALobbyPlayerController : public ACustomPlayerControllerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
private:
	/*
     * Widget & Menu Functionality
	 */
	
	virtual void CreateWidgetObjects();
	virtual void AddWidgetsToViewport();

	//
	// EscapeMenu Widget
	
	virtual void ToggleEscapeMenu() override;
	UPROPERTY()
	class UEscapeMenuWidgetBase* EscapeMenuBaseWidget;
};
