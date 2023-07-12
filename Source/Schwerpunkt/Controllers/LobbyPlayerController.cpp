// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "Schwerpunkt/HUD/EscapeMenuWidgetBase.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateWidgetObjects();
	AddWidgetsToViewport();
}

void ALobbyPlayerController::CreateWidgetObjects()
{
	if (EscapeMenuBaseWidgetClass && IsLocalController())
	{
		EscapeMenuBaseWidget = CreateWidget<UEscapeMenuWidgetBase>(this, EscapeMenuBaseWidgetClass);
	}
}

void ALobbyPlayerController::AddWidgetsToViewport()
{
	if (EscapeMenuBaseWidget)
	{
		EscapeMenuBaseWidget->AddToViewport();
		EscapeMenuBaseWidget->bIsFocusable = true;
		EscapeMenuBaseWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ALobbyPlayerController::ToggleEscapeMenu()
{
	Super::ToggleEscapeMenu();

	if (!EscapeMenuBaseWidget) return;

	if (!EscapeMenuBaseWidget->IsVisible())
	{
		EscapeMenuBaseWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(EscapeMenuBaseWidget->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(true);
		SetInputMode(InputModeData);
		SetShowMouseCursor(true);
	}
	else
	{
		EscapeMenuBaseWidget->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputModeData;
		InputModeData.SetConsumeCaptureMouseDown(false);
		SetInputMode(InputModeData);
		SetShowMouseCursor(false);
	}
}
