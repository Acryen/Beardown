// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscapeMenuWidgetBase.h"
#include "EscapeMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API UEscapeMenuWidget : public UEscapeMenuWidgetBase
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	
private:	
	UPROPERTY(meta = (BindWidget))
	class UButton* RematchButton;

	UFUNCTION()
	void OnRematchButtonClicked();
	
	UPROPERTY()
	class AFlipperPlayerController* FlipperPlayerController;
};
