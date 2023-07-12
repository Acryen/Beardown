// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscapeMenuWidgetBase.generated.h"


UCLASS()
class SCHWERPUNKT_API UEscapeMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	// Unfortunately, OnLevelRemovedFromWorld is also called when a second player joined. Cannot be called up in multiplayer maps in a meaningful way.
	// virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	
	UPROPERTY(meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void OnResumeButtonClicked();
	UFUNCTION()
	void OnLobbyButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();

	void MenuTearDown();
	
	UPROPERTY()
	class ACustomPlayerControllerBase* CustomPlayerController;
	UPROPERTY()
	class UMultiplayerGameInstanceSubsystem* MultiplayerGameInstanceSubsystem;
};
