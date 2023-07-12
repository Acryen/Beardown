// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API UFlipperOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UpperText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MiddleText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;
};
