// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerControllerBase.h"
#include "FlipperPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API AFlipperPlayerController : public ACustomPlayerControllerBase
{
	GENERATED_BODY()
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
private:
	/*
	 * Widget & Menu Functionality
	 */
	
	void CreateWidgetObjects();
	void AddWidgetsToViewport();

	//
	// FlipperOverlayWidget - Functions and Variables
	
	void FlipperOverlayHideScoreText();
	void FlipperOverlayHideUpperText();
	void FlipperOverlayHideMiddleText();
	UPROPERTY()
	class UFlipperOverlayWidget* FlipperOverlayWidget;

	//
	// FlipperOverlayWidget - Blueprint Editables
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> FlipperOverlayWidgetClass;

public:
	//
	// EscapeMenuWidget - Functions and Variables
	
	virtual void ToggleEscapeMenu() override;
	UFUNCTION(Server, Reliable)
	void ServerRequestRematch();

private:
	UPROPERTY()
	class UEscapeMenuWidget* EscapeMenuWidget;

public:
	/*
	 * Starting Text functionality
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRefreshCountdownWidget(int32 NewCountdownNumber);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartGame();

private:
	bool bWelcomeTextNotChanged = true;

	//
	// Starting Text functionality - Editables
	
	UPROPERTY(EditDefaultsOnly)
	float HideStartingTextDelay = 1.5f;
	UPROPERTY(EditDefaultsOnly)
	float GoFontSize = 100.f;

public:
	/*
	 * Goal and Score Text functionality
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGoalScored(int32 ScorePlayerOne, int32 ScorePlayerTwo);

private:
	//
	// Goal and Score TExt functionality - Editables
	
	UPROPERTY(EditDefaultsOnly)
	float GoalFontSize = 120.f;
	UPROPERTY(EditDefaultsOnly)
	float HideGoalTextDelay = 3.f;
	UPROPERTY(EditDefaultsOnly)
	float HideScoreTextDelay = 10.f;

public:
	/*
	 * Stage Three functionality
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartCameraShake();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartCameraBlend();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopCameraBlend();

private:
	void ReEnableInput();

	//
	// Stage Three functionality - Editables
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShakeBlueprint;
	UPROPERTY(EditAnywhere, Category = CameraBlend)
	float CameraBlendDuration = 2.f;

public:
	/*
	 * Win/End of Game functionality
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEndOfGameInitiation(bool bRedPlayerHasWon);
	UPROPERTY(Replicated)
	bool bIsRedPlayer = true;
	int32 TeamNumber;

private:
	bool bRedPlayerWon;
	void EndOfGame();
	void FadeIn();
	UFUNCTION(Server, Reliable)
	void ServerSetPawnLocationToEndOfGameStage();
	void SetPawnRotationToEndOfGameState();
	void DisablePhysicsInteraction(); 
	void ChangeViewTarget();

	//
	// Win/End of Game functionality - Editables - Durations
	
	UPROPERTY(EditDefaultsOnly, Category = EndOfGame)
	float HoldFadeOutDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = EndOfGame)
	float DisabledInputDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = EndOfGame)
	float DisplayTeamWonDuration = 3.f;

	//
	// Win/End of Game functionality - Editables - Podium & Cage Positions
	
	UPROPERTY(EditDefaultsOnly, Category = EndOfGamePositions)
	FVector PodiumPositionOne;
	UPROPERTY(EditDefaultsOnly, Category = EndOfGamePositions)
	FVector PodiumPositionTwo;
	UPROPERTY(EditDefaultsOnly, Category = EndOfGamePositions)
	FVector CagePositionOne;
	UPROPERTY(EditDefaultsOnly, Category = EndOfGamePositions)
	FVector CagePositionTwo;

	/*
	 * Debug
	 */
	
	UFUNCTION(Exec)
	void GetTeam();
};
