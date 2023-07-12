// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FlipperGameMode.generated.h"

UENUM()
enum class EWhatTeamWon : uint8
{
	EWTW_NoWinner UMETA(DisplayName = "No Winner"),
	EWTW_RedTeamWon UMETA(DisplayName = "Red Team Won"),
	EWTW_BlueTeamWon UMETA(DisplayName = "Blue Team Won"),

	EWTW_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SCHWERPUNKT_API AFlipperGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	/*
	 * Handle First Time Spawning the Players
	 */
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void PlayerHasArrived(class AFlipperPlayerController* PlayerController);
	virtual void Logout(AController* Exiting) override;

private:
	int32 NrOfSpawnedPlayer = 0;
	int32 BlueTeamNumber = 1;
	int32 RedTeamNumber = 1;
	
public:
	/*
	 * UI Functionality
	 */
	
	void ReceiveRematchRequest();

private:
	int32 RematchRequests = 0;
	void StartCountdown();
	
	/*
	 * Starting Countdown
	 */
	
	FTimerHandle TimerHandle;

	//
	// Startint Countdown - Blueprint Editables
	
	UPROPERTY(EditDefaultsOnly)
	float CountdownStartDelay = 1.5f;
	UPROPERTY(EditDefaultsOnly)
	int32 CountdownNumber = 4;

public:
	/*
	 * Goal scored functionality
	 */
	
	void GoalScored(bool bPlayerOneHasScored);
	EWhatTeamWon CheckIfPlayerHasWon();

private:
	void CalculateTransitionNumbers();
	int32 GoalsForTransitionOne;
	int32 GoalsForTransitionTwo;
	int32 GoalsScored = 0;
	int32 GoalsForPlayerToWin = 2;
	
	/*
	 * StageManager and corresponding Arrays
	 */
	
	UPROPERTY()
	class AStageManager* StageManager;
	UPROPERTY()
	TArray<class AFlipperLever*> StartingLeverArray;
	UPROPERTY()
	TArray<class AFlipperLever*> StageOneLeverArray;
	UPROPERTY()
	TArray<class AFlipperLever*> StageTwoLeverArray;
	UPROPERTY()
	TArray<class AFlipperLever*> KeeperLeverArray;
	
public:	
	/*
	 * Miscellaneous
	 */
	
	void KillPlayerByKillZ(APawn* PlayerPawn);

private:
	void StartEndOfGameInitiation();
	UPROPERTY()
	TArray<class AFlipperPlayerController*> PlayerControllerArray;
	UPROPERTY()
	class AFlipperGameState* FlipperGameState;
	UPROPERTY()
	class USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem;
	float StartStageThreeDelay = 4.f;

	//
	// Miscellaneous - Blueprint Editables
	
	UPROPERTY(EditDefaultsOnly)
	float RaiseEndOfGameStageDelay = 8.25f;

public:
	FORCEINLINE void SetStageManager(class AStageManager* NewStageManager) { StageManager = NewStageManager; }
	FORCEINLINE void AddStartingLever(class AFlipperLever* FlipperLever) { StartingLeverArray.Add(FlipperLever); }
	FORCEINLINE void AddStageOneFlipperLever(class AFlipperLever* FlipperLever) { StageOneLeverArray.Add(FlipperLever); }
	FORCEINLINE void AddStageTwoFlipperLever(class AFlipperLever* FlipperLever) { StageTwoLeverArray.Add(FlipperLever); }
	FORCEINLINE void AddKeeperFlipperLever(class AFlipperLever* FlipperLever) { KeeperLeverArray.Add(FlipperLever); }
	FORCEINLINE void SetStartStageThreeDelay(float FlipperBallRespawnDelay) { StartStageThreeDelay = FlipperBallRespawnDelay; }
	FORCEINLINE TArray<class AFlipperPlayerController*> GetAllPlayerControllers() { return PlayerControllerArray; }

	/*
	 * Debug
	 */
	
	UFUNCTION(Exec)
	void ScoreGoal(bool bScoreForRed);
	UFUNCTION(Exec)
	void StartPlayerWon();
	UFUNCTION(Exec)
	void RestartFlipperLevel();
	UFUNCTION(Exec)
	void SetGoalsForWin(int32 GoalsAmount);
};