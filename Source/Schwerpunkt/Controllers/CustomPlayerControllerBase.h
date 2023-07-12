// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API ACustomPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

public:
	//
	// EscapeMenuWidget
	
	FORCEINLINE virtual void ToggleEscapeMenu() {}
	
protected:
	UPROPERTY(EditAnywhere)
	USoundBase* OpenCloseSound;

	//
	//EscapeMenuWidget - Blueprint Editables
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEscapeMenuWidgetBase> EscapeMenuBaseWidgetClass;

public:
	/*
	 * Miscellaneous
	 */
	
	UFUNCTION(Server, Reliable)
	void ServerDestroyPawnsWeapon();
};
