// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/OnlineSessionInterface.h"

#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	int32 NumPublicConnections{4}; // <- Is currently no longer used and the values are hardcoded in the .cpp file when calling CreateSession()
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};

	int32 RequiredGoalsToWin = 2;

	// The subsystem designed to handle all online session functionality.
	// This is our custom subsystem, derived from UGameInstanceSubsystem.
	UPROPERTY()
	class UMultiplayerGameInstanceSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UButton* Host1v1Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* Host2v2Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RequiredGoalsText;

	UPROPERTY(meta = (BindWidget))
	class UButton* IncreaseGoalsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DecreaseGoalsButton;

	UFUNCTION()
	void Host1v1ButtonClicked();

	UFUNCTION()
	void Host2v2ButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void QuitButtonClicked();

	UFUNCTION()
	void IncreaseGoalsButtonClicked();

	UFUNCTION()
	void DecreaseGoalsButtonClicked();

	void MenuTearDown();
};
