// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Schwerpunkt/Character/FlipperGameCharacter.h"
#include "Schwerpunkt/Controllers/FlipperPlayerController.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"
#include "Schwerpunkt/GameStates/FlipperGameState.h"
#include "Schwerpunkt/Miscellaneous/StageManager.h"

// This function is called before PlayerSpawn and the return value is the selected spawn for the player.
AActor* AFlipperGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* StandardPlayerStart = Super::ChoosePlayerStart_Implementation(Player);

	// If there are 2 PlayerStarts, return the first PlayerStart to the first/third player and the second to the second/fourth player.
	TArray<AActor*> PlayerStartsArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartsArray);
	if (PlayerStartsArray.Num() < 2)
	{
		return StandardPlayerStart;
	}
	
	NrOfSpawnedPlayer++;
	if (NrOfSpawnedPlayer % 2 == 1)
	{
		AFlipperPlayerController* FlipperPlayerController = Cast<AFlipperPlayerController>(Player);
		if (FlipperPlayerController)
		{
			FlipperPlayerController->TeamNumber = RedTeamNumber;
		}
			
		RedTeamNumber++;
		
		return PlayerStartsArray[0];
	}
	else
	{
		// bIsRedPlayer is true by default and does not have to be set to true again in the statement above.
		AFlipperPlayerController* FlipperPlayerController = Cast<AFlipperPlayerController>(Player);
		if (FlipperPlayerController)
		{
			FlipperPlayerController->bIsRedPlayer = false;
			FlipperPlayerController->TeamNumber = BlueTeamNumber;
		}
		BlueTeamNumber++;
		
		return PlayerStartsArray[1];
	}
}

void AFlipperGameMode::BeginPlay()
{
	Super::BeginPlay();

	FlipperGameState = GetGameState<AFlipperGameState>();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
	}
	
	if (!SchwerpunktGameInstanceSubsystem) return;
	GoalsForPlayerToWin = SchwerpunktGameInstanceSubsystem->RequiredGoalsToWin;
	CalculateTransitionNumbers();

	// TODO Debug Message
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Required Goals To Win: %i"), GoalsForPlayerToWin));
}

void AFlipperGameMode::CalculateTransitionNumbers()
{
	GoalsForTransitionOne = GoalsForPlayerToWin - 1;
	GoalsForTransitionTwo = GoalsForTransitionOne * 2;
}

// Called by the PlayerController
void AFlipperGameMode::PlayerHasArrived(AFlipperPlayerController* PlayerController)
{
	PlayerControllerArray.Add(PlayerController);

	if (!SchwerpunktGameInstanceSubsystem) return;
	if (PlayerControllerArray.Num() >= SchwerpunktGameInstanceSubsystem->SessionMaxPlayers)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperGameMode::StartCountdown, 1.f, true, CountdownStartDelay);
	}
}

void AFlipperGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// TODO Debug Message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Logout wurde aufgerufen"));
}

void AFlipperGameMode::StartCountdown()
{	
	for (AFlipperPlayerController* Controller : PlayerControllerArray)
	{
		if (CountdownNumber > 0)
		{
			Controller->MulticastRefreshCountdownWidget(CountdownNumber);
		}
		else
		{
			Controller->MulticastStartGame();

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
	}		
	CountdownNumber--;
}

// Increase the score, display appropriate text on the screen and when a player has scored enough goals, fire the EndOfGame() function in the PlayerController.
void AFlipperGameMode::GoalScored(bool bPlayerOneHasScored)
{
	if (!FlipperGameState) return;
	FlipperGameState->IncreaseScore(bPlayerOneHasScored);

	for (AFlipperPlayerController* Controller : PlayerControllerArray)
	{
		Controller->MulticastGoalScored(FlipperGameState->GetPlayerOneScore(), FlipperGameState->GetPlayerTwoScore());
	}

	GoalsScored++;
	if (CheckIfPlayerHasWon() == EWhatTeamWon::EWTW_NoWinner)
	{
		if (!StageManager)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("StageManager Variable in FlipperGameMode is nullptr"));
			return;
		}

		if (GoalsScored == GoalsForTransitionOne)
		{
			StageManager->ActivateStageTwo(StartingLeverArray, StageOneLeverArray, StageTwoLeverArray, KeeperLeverArray);
		}
		else if (GoalsScored == GoalsForTransitionTwo)
		{
			StageManager->ActivateStageThree(StartStageThreeDelay);
		}
	}
	else
	{
		StartEndOfGameInitiation();
	}
}

// Returned true if a player should have scored enough goals.
EWhatTeamWon AFlipperGameMode::CheckIfPlayerHasWon()
{
	if (!FlipperGameState) return EWhatTeamWon::EWTW_NoWinner;

	bool bRedTeamWon = FlipperGameState->GetPlayerOneScore() >= GoalsForPlayerToWin;
	bool bBlueTeamWon = FlipperGameState->GetPlayerTwoScore() >= GoalsForPlayerToWin;

	if (bRedTeamWon)
	{
		return EWhatTeamWon::EWTW_RedTeamWon;
	}

	if (bBlueTeamWon)
	{
		return EWhatTeamWon::EWTW_BlueTeamWon;
	}
	
	return EWhatTeamWon::EWTW_NoWinner;
}

void AFlipperGameMode::StartEndOfGameInitiation()
{
	for (AFlipperPlayerController* Controller : PlayerControllerArray)
	{
		CheckIfPlayerHasWon() == EWhatTeamWon::EWTW_RedTeamWon ? Controller->MulticastEndOfGameInitiation(true) : Controller->MulticastEndOfGameInitiation(false);
	}
	
	if (!StageManager) return;
	StageManager->bRedTeamHasWon = CheckIfPlayerHasWon() == EWhatTeamWon::EWTW_RedTeamWon;
	
	if (GoalsScored <= GoalsForTransitionTwo) // <= Since the goal for the transition can be the winning goal, which does not initiate the transition
	{
		FTimerHandle TimerHandleOne;
		GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, StageManager, &AStageManager::MulticastStageTwoToEndTransition, RaiseEndOfGameStageDelay);
	}
	else
	{
		FTimerHandle TimerHandleOne;
		GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, StageManager, &AStageManager::MulticastStageThreeToEndTransition, RaiseEndOfGameStageDelay);
	}
}

void AFlipperGameMode::KillPlayerByKillZ(APawn* PlayerPawn)
{
	AFlipperPlayerController* FlipperPlayerController = Cast<AFlipperPlayerController>(PlayerPawn->GetController());
	AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(PlayerPawn);
	if (!FlipperPlayerController || !FlipperGameCharacter) return;

	FlipperGameCharacter->DestroyWeapon();
	FlipperGameCharacter->Reset();
	RestartPlayer(FlipperPlayerController);

	FlipperGameCharacter = Cast<AFlipperGameCharacter>(FlipperPlayerController->GetPawn());
	if (!FlipperGameCharacter) return;
	FlipperGameCharacter->ServerSetupClothingMaterials(FlipperPlayerController->bIsRedPlayer);
}

void AFlipperGameMode::ReceiveRematchRequest()
{
	RematchRequests++;
	if (RematchRequests == SchwerpunktGameInstanceSubsystem->SessionMaxPlayers)
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/RestartMap?listen"));
	}
}

/*
 *
 * TODO Debug functions
 * 
 */
void AFlipperGameMode::ScoreGoal(bool bScoreForRed)
{
	GoalScored(bScoreForRed);
}

void AFlipperGameMode::StartPlayerWon()
{
	StartEndOfGameInitiation();
}

void AFlipperGameMode::RestartFlipperLevel()
{
	GetWorld()->ServerTravel(FString("/Game/Maps/RestartMap?listen"));
}

void AFlipperGameMode::SetGoalsForWin(int32 GoalsAmount)
{
	GoalsForPlayerToWin = GoalsAmount;
}
