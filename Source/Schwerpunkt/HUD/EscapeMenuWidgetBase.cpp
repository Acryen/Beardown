// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeMenuWidgetBase.h"

#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Schwerpunkt/Controllers/CustomPlayerControllerBase.h"
#include "Schwerpunkt/GameInstances/MultiplayerGameInstanceSubsystem.h"

bool UEscapeMenuWidgetBase::Initialize()
{
	Super::Initialize();

	// Setup Buttons
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UEscapeMenuWidgetBase::OnResumeButtonClicked);
	}

	if (LobbyButton)
	{
		LobbyButton->OnClicked.AddDynamic(this, &UEscapeMenuWidgetBase::OnLobbyButtonClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UEscapeMenuWidgetBase::OnQuitButtonClicked);
	}
	
	// Setup PlayerController Variable
	CustomPlayerController = Cast<ACustomPlayerControllerBase>(GetOwningPlayer());

	// Setup MultiplayerGameInstanceSubsystem
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerGameInstanceSubsystem = GameInstance->GetSubsystem<UMultiplayerGameInstanceSubsystem>();
		if (MultiplayerGameInstanceSubsystem)
		{
			MultiplayerGameInstanceSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UEscapeMenuWidgetBase: MultiplayerGameInstanceSubsystem not found!"));
		}
	}
	
	return true;
}

void UEscapeMenuWidgetBase::OnResumeButtonClicked()
{
	if (CustomPlayerController)
	{
		CustomPlayerController->ToggleEscapeMenu();
	}
}

void UEscapeMenuWidgetBase::OnLobbyButtonClicked()
{
	LobbyButton->SetIsEnabled(false);
	
	if (MultiplayerGameInstanceSubsystem)
	{
		MultiplayerGameInstanceSubsystem->DestroySession();
	}
}

void UEscapeMenuWidgetBase::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		LobbyButton->SetIsEnabled(true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UEscapeMenuWidgetBase::OnDestroySession() early return"));
		return;
	}

	MenuTearDown();
	
	AGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<AGameModeBase>();
	if (GameModeBase)
	{
		GameModeBase->ReturnToMainMenuHost();

		if (CustomPlayerController)
		{
			CustomPlayerController->ServerDestroyPawnsWeapon();
		}
	}
	else
	{
		if (CustomPlayerController)
		{
			CustomPlayerController->ClientReturnToMainMenuWithTextReason(FText());
			CustomPlayerController->ServerDestroyPawnsWeapon();
		}
	}
}

void UEscapeMenuWidgetBase::OnQuitButtonClicked()
{
	if (CustomPlayerController)
	{
		CustomPlayerController->ServerDestroyPawnsWeapon();
	}
	
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(),TEnumAsByte<EQuitPreference::Type>(0) , true);
}

void UEscapeMenuWidgetBase::MenuTearDown()
{
	RemoveFromParent();

	if (!CustomPlayerController) return;
	FInputModeGameOnly InputModeData;
	CustomPlayerController->SetInputMode(InputModeData);
	CustomPlayerController->SetShowMouseCursor(false);
}