// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPlayerController.h"

#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Schwerpunkt/Character/FlipperGameCharacter.h"
#include "Schwerpunkt/GameInstances/SchwerpunktGameInstanceSubsystem.h"
#include "Schwerpunkt/GameModes/FlipperGameMode.h"
#include "Schwerpunkt/HUD/EscapeMenuWidget.h"
#include "Schwerpunkt/HUD/FlipperOverlayWidget.h"


void AFlipperPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperPlayerController, bIsRedPlayer);
}

void AFlipperPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateWidgetObjects();
	AddWidgetsToViewport();

	AFlipperGameCharacter* PlayerCharacter = Cast<AFlipperGameCharacter>(GetPawn());
	if (PlayerCharacter && !PlayerCharacter->GetIsLobbyCharacter())
	{
		PlayerCharacter->ServerDisablePlayerMovement();
		PlayerCharacter->ServerSetupClothingMaterials(bIsRedPlayer);
	}
	
	if (!HasAuthority()) return;
	AFlipperGameMode* FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	if (FlipperGameMode)
	{
		FlipperGameMode->PlayerHasArrived(this);
	}
}

void AFlipperPlayerController::CreateWidgetObjects()
{	
	if (!IsLocalController()) return;
	
	if (FlipperOverlayWidgetClass)
	{
		FlipperOverlayWidget = CreateWidget<UFlipperOverlayWidget>(this, FlipperOverlayWidgetClass);
	}
	
	if (EscapeMenuBaseWidgetClass)
	{
		EscapeMenuWidget = CreateWidget<UEscapeMenuWidget>(this, EscapeMenuBaseWidgetClass);
	}
}

void AFlipperPlayerController::AddWidgetsToViewport()
{
	if (FlipperOverlayWidget)
	{
		FlipperOverlayWidget->AddToViewport();
	}
	
	if (EscapeMenuWidget)
	{
		EscapeMenuWidget->bIsFocusable = true;
		EscapeMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		EscapeMenuWidget->AddToViewport();
	}
}

void AFlipperPlayerController::ToggleEscapeMenu()
{
	Super::ToggleEscapeMenu();

	if (!EscapeMenuWidget) return;

	if (OpenCloseSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), OpenCloseSound);
	}
	
	if (!EscapeMenuWidget->IsVisible())
	{
		EscapeMenuWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(EscapeMenuWidget->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(true);
		SetInputMode(InputModeData);
		SetShowMouseCursor(true);
	}
	else
	{
		EscapeMenuWidget->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputModeData;
		InputModeData.SetConsumeCaptureMouseDown(false);
		SetInputMode(InputModeData);
		SetShowMouseCursor(false);
	}
}

void AFlipperPlayerController::ServerRequestRematch_Implementation()
{
	AFlipperGameMode* FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	if (FlipperGameMode)
	{
		FlipperGameMode->ReceiveRematchRequest();
	}
}

// This function is called by GameMode
void AFlipperPlayerController::MulticastRefreshCountdownWidget_Implementation(int32 NewCountdownNumber)
{
	bool bOverlayWidgetIsValid = (
		FlipperOverlayWidget &&
		FlipperOverlayWidget->UpperText &&
		FlipperOverlayWidget->MiddleText);
	if (!bOverlayWidgetIsValid) return;

	FlipperOverlayWidget->MiddleText->SetText(FText::FromString(FString::Printf(TEXT("%i"), NewCountdownNumber)));

	if (bWelcomeTextNotChanged)
	{
		FlipperOverlayWidget->UpperText->SetText(FText::FromString(TEXT("Game starts in:")));
		bWelcomeTextNotChanged = false;
	}
}

// This function is called by GameMode. Widget will be adjusted for game launch and PlayerMovement will be released.
void AFlipperPlayerController::MulticastStartGame_Implementation()
{
	/*
	 * Edit Widgets 
	 */
	
	bool bOverlayWidgetIsValid = (
		FlipperOverlayWidget &&
		FlipperOverlayWidget->UpperText &&
		FlipperOverlayWidget->MiddleText);
	
	if (bOverlayWidgetIsValid)
	{
		FlipperOverlayHideUpperText();
		
		FSlateFontInfo FontInfo = FlipperOverlayWidget->MiddleText->Font;
		FontInfo.Size = GoFontSize;
		FlipperOverlayWidget->MiddleText->SetFont(FontInfo);
		FlipperOverlayWidget->MiddleText->SetText(FText::FromString(FString(TEXT("Go!"))));
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperPlayerController::FlipperOverlayHideMiddleText, HideStartingTextDelay);

	/*
	 * Unlock PlayerMovement
	 */
	
	AFlipperGameCharacter* PlayerCharacter = Cast<AFlipperGameCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->EnablePlayerMovement();
	}
}

void AFlipperPlayerController::FlipperOverlayHideUpperText()
{
	if (FlipperOverlayWidget && FlipperOverlayWidget->UpperText)
	{
		FlipperOverlayWidget->UpperText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AFlipperPlayerController::FlipperOverlayHideMiddleText()
{
	if (FlipperOverlayWidget && FlipperOverlayWidget->MiddleText)
	{
		FlipperOverlayWidget->MiddleText->SetVisibility(ESlateVisibility::Hidden);		
	}
}

void AFlipperPlayerController::MulticastGoalScored_Implementation(int32 ScorePlayerOne, int32 ScorePlayerTwo)
{
	bool OverlayWidgetIsValid = (
		FlipperOverlayWidget &&
		FlipperOverlayWidget->MiddleText &&
		FlipperOverlayWidget->ScoreText);
	if (!OverlayWidgetIsValid) return;

	FlipperOverlayWidget->MiddleText->SetVisibility(ESlateVisibility::Visible);
	FlipperOverlayWidget->MiddleText->SetText(FText::FromString(FString(TEXT("Goal!"))));

	FSlateFontInfo FontInfo = FlipperOverlayWidget->MiddleText->Font;
	FontInfo.Size = GoalFontSize;
	FlipperOverlayWidget->MiddleText->SetFont(FontInfo);

	FlipperOverlayWidget->ScoreText->SetVisibility(ESlateVisibility::Visible);
	FlipperOverlayWidget->ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score\n%i : %i"), ScorePlayerOne, ScorePlayerTwo)));

	FTimerHandle TimerHandleOne;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &AFlipperPlayerController::FlipperOverlayHideMiddleText, HideGoalTextDelay);

	FTimerHandle TimerHandleTwo;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleTwo, this, &AFlipperPlayerController::FlipperOverlayHideScoreText, HideScoreTextDelay);
}

void AFlipperPlayerController::FlipperOverlayHideScoreText()
{
	if (FlipperOverlayWidget && FlipperOverlayWidget->ScoreText)
	{
		FlipperOverlayWidget->ScoreText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AFlipperPlayerController::MulticastStartCameraShake_Implementation()
{
	if (!PlayerCameraManager || !CameraShakeBlueprint) return;
	
	PlayerCameraManager->StartCameraShake(CameraShakeBlueprint);
}

void AFlipperPlayerController::MulticastStartCameraBlend_Implementation()
{
	TArray<AActor*> BlendTargetArray;
	UGameplayStatics::GetAllActorsWithTag(this, FName(FString(TEXT("StageThreeViewTarget"))), BlendTargetArray);

	if (BlendTargetArray.IsEmpty()) return;
	SetViewTargetWithBlend(BlendTargetArray[0], CameraBlendDuration);

	if (GetPawn())
	{
		GetPawn()->DisableInput(this);
	}
}

void AFlipperPlayerController::MulticastStopCameraBlend_Implementation()
{
	SetViewTargetWithBlend(GetPawn(), CameraBlendDuration);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperPlayerController::ReEnableInput, CameraBlendDuration);
}

void AFlipperPlayerController::ReEnableInput()
{
	if (GetPawn())
	{
		GetPawn()->EnableInput(this);
	}
}

void AFlipperPlayerController::MulticastEndOfGameInitiation_Implementation(bool bRedPlayerHasWon)
{
	bRedPlayerWon = bRedPlayerHasWon;
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperPlayerController::EndOfGame, HideGoalTextDelay + 0.25f);
}

void AFlipperPlayerController::EndOfGame()
{
	if (GetPawn())
	{
		GetPawn()->DisableInput(this);
	}
	
	PlayerCameraManager->StartCameraFade(0.f, 1.f, 2.f, FLinearColor::Black, false, true);
	FTimerHandle TimerHandleOne;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &AFlipperPlayerController::FadeIn, HoldFadeOutDuration);
}

void AFlipperPlayerController::FadeIn()
{
	ServerSetPawnLocationToEndOfGameStage(); // Only the server has the right boolean values for the right placement.
	SetPawnRotationToEndOfGameState(); // Changes in the rotation are somehow not replicated, so the client has to do it again...
	DisablePhysicsInteraction(); 
	ChangeViewTarget();
	PlayerCameraManager->StartCameraFade(1.f, 0.f, 2.f, FLinearColor::Black);

	if (FlipperOverlayWidget && FlipperOverlayWidget->UpperText)
	{
		FString WhichPlayerWinString = FString::Printf(TEXT("%s Team Won!!"), bRedPlayerWon ? TEXT("Red") : TEXT("Blue"));
		FlipperOverlayWidget->UpperText->SetText(FText::FromString(WhichPlayerWinString));
		FlipperOverlayWidget->UpperText->SetVisibility(ESlateVisibility::Visible);
	}
	FTimerHandle TimerHandleTwo;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleTwo, this, &ThisClass::FlipperOverlayHideUpperText, DisplayTeamWonDuration);

	FTimerHandle TimerHandleOne;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleOne, this, &ThisClass::MulticastStopCameraBlend, DisabledInputDuration);
}

void AFlipperPlayerController::ServerSetPawnLocationToEndOfGameStage_Implementation()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance || !GetPawn()) return;
	USchwerpunktGameInstanceSubsystem* SchwerpunktGameInstanceSubsystem = GameInstance->GetSubsystem<USchwerpunktGameInstanceSubsystem>();
	
	// When we win, it's off to the podium
	if ((bRedPlayerWon && bIsRedPlayer) || (!bRedPlayerWon && !bIsRedPlayer))
	{
		if (SchwerpunktGameInstanceSubsystem && SchwerpunktGameInstanceSubsystem->SessionMaxPlayers == 2)
		{
			// We get into the block when it's a 1v1 session. Accordingly, the player should be in the middle of the podium.
			FVector Direction = (PodiumPositionOne - PodiumPositionTwo) / 2;
			FVector TargetLocation = PodiumPositionTwo + Direction;
			GetPawn()->SetActorLocation(TargetLocation);
		}
		else // means that this is a 2v2 session
		{
			if (TeamNumber == 1)
			{
				GetPawn()->SetActorLocation(PodiumPositionOne);
			}
			else
			{
				GetPawn()->SetActorLocation(PodiumPositionTwo);
			}
		}
	}
	else // If we lose, off to the cage
	{
		if (SchwerpunktGameInstanceSubsystem && SchwerpunktGameInstanceSubsystem->SessionMaxPlayers == 2)
		{
			// We get into the block when it's a 1v1 session. Accordingly, the player should be in the middle of the cage.
			FVector Direction = (CagePositionOne - CagePositionTwo) / 2;
			FVector TargetLocation = CagePositionTwo + Direction;
			GetPawn()->SetActorLocation(TargetLocation);
		}
		else // means that this is a 2v2 session
		{
			if (TeamNumber == 1)
			{
				GetPawn()->SetActorLocation(CagePositionOne);
			}
			else
			{
				GetPawn()->SetActorLocation(CagePositionTwo);
			}
		}
	}
}

void AFlipperPlayerController::SetPawnRotationToEndOfGameState()
{
	GetPawn()->SetActorRotation(FRotator::MakeFromEuler(FVector(0, 0, 180)));
}

void AFlipperPlayerController::DisablePhysicsInteraction()
{
	/* This function is called from a multicast function.
	 * On the server there is a PlayerController for each Pawn and this function disables PhysicsInteraction for each PlayerPawn there.
	 * However, the corresponding boolean in the CharacterMovementComponent is apparently not replicated.
	 * Simply making a MulticastFunction from this function in the controller is unfortunately not enough,
	 * since the changes in the MovementComponent are not forwarded to the simulated proxies, since there are no PlayerControllers on the clients that execute this function here.
	 * Therefore a multicast function in the character. */
	if (!HasAuthority()) return;
	AFlipperGameCharacter* PlayerCharacter = Cast<AFlipperGameCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->MulticastDisablePhysicsInteraction();
	}
}

void AFlipperPlayerController::ChangeViewTarget()
{
	TArray<AActor*> EndOfGameViewTargetArray;
	UGameplayStatics::GetAllActorsWithTag(this, FName(TEXT("EndOfGameViewTarget")), EndOfGameViewTargetArray);
	if (EndOfGameViewTargetArray.IsEmpty()) return;
	
	SetViewTarget(EndOfGameViewTargetArray[0]);
	ClientSetRotation(EndOfGameViewTargetArray[0]->GetActorRotation());
}

void AFlipperPlayerController::GetTeam()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), bIsRedPlayer ? TEXT("true") : TEXT("false"));
}
