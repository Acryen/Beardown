// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Schwerpunkt/GameInstances/MultiplayerGameInstanceSubsystem.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"

void UMainMenuWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	PathToLobby = FString(TEXT("/Game/Maps/LobbyJasper?listen"));

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerGameInstanceSubsystem>();
	}
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMainMenuWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (!Host1v1Button) return false;
	Host1v1Button->OnClicked.AddDynamic(this, &ThisClass::Host1v1ButtonClicked);
	
	if (!Host2v2Button) return false;
	Host2v2Button->OnClicked.AddDynamic(this, &ThisClass::Host2v2ButtonClicked);
	
	if (!JoinButton) return false;
	JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);

	if (!QuitButton) return false;
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::QuitButtonClicked);

	if (!IncreaseGoalsButton) return false;
	IncreaseGoalsButton->OnClicked.AddDynamic(this, &ThisClass::IncreaseGoalsButtonClicked);

	if (!DecreaseGoalsButton) return false;
	DecreaseGoalsButton->OnClicked.AddDynamic(this, &ThisClass::DecreaseGoalsButtonClicked);

	return  true;
}

void UMainMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	MenuTearDown();
}

void UMainMenuWidget::Host1v1ButtonClicked()
{
	Host1v1Button->SetIsEnabled(false);
	Host2v2Button->SetIsEnabled(false);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
		if (SchwerpunktGameInstanceSubsystem)
		{
			SchwerpunktGameInstanceSubsystem->SessionMaxPlayers = 2;
			SchwerpunktGameInstanceSubsystem->bAlreadyJoinedSession = true;
		}
	}
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(2, MatchType);
	}
}

void UMainMenuWidget::Host2v2ButtonClicked()
{
	Host1v1Button->SetIsEnabled(false);
	Host2v2Button->SetIsEnabled(false);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
		if (SchwerpunktGameInstanceSubsystem)
		{
			SchwerpunktGameInstanceSubsystem->SessionMaxPlayers = 4;
			SchwerpunktGameInstanceSubsystem->bAlreadyJoinedSession = true;
		}
	}
		
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(4, MatchType);
	}
}

void UMainMenuWidget::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString(TEXT("Session created successfully!")));
		}
		
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
			if (SchwerpunktGameInstanceSubsystem)
			{
				SchwerpunktGameInstanceSubsystem->bAlreadyJoinedSession = true;
				SchwerpunktGameInstanceSubsystem->RequiredGoalsToWin = RequiredGoalsToWin;
			}
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}
		
		Host1v1Button->SetIsEnabled(true);
		Host2v2Button->SetIsEnabled(true);
	}
}

void UMainMenuWidget::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMainMenuWidget::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(),TEnumAsByte<EQuitPreference::Type>(0) , true);
}

void UMainMenuWidget::IncreaseGoalsButtonClicked()
{
	RequiredGoalsToWin++;
	
	if (RequiredGoalsText)
	{
		RequiredGoalsText->SetText(FText::FromString(FString::Printf(TEXT("%i"), RequiredGoalsToWin)));
	}
}

void UMainMenuWidget::DecreaseGoalsButtonClicked()
{
	if (RequiredGoalsToWin > 1)
	{
		RequiredGoalsToWin--;	
	}
	
	if (RequiredGoalsText)
	{
		RequiredGoalsText->SetText(FText::FromString(FString::Printf(TEXT("%i"), RequiredGoalsToWin)));
	}
}

void UMainMenuWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem) return;

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMainMenuWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);

		// TODO Debug messages
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MainMenuWidget: AlreadyInSession"));
			break;
			
		case EOnJoinSessionCompleteResult::SessionIsFull:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MainMenuWidget: SessionIsFull"));
			break;
			
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MainMenuWidget: CouldNotRetrieveAdress"));
			break;
			
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MainMenuWidget: SessionDoesNotExist"));
			break;
			
		case EOnJoinSessionCompleteResult::UnknownError:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MainMenuWidget: UnknownError"));

		default:
			break;
		}
	}
}

void UMainMenuWidget::OnDestroySession(bool bWasSuccessful)
{
}

void UMainMenuWidget::OnStartSession(bool bWasSuccessful)
{
}

void UMainMenuWidget::MenuTearDown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}