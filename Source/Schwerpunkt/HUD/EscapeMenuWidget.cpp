// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeMenuWidget.h"

#include "Components/Button.h"
#include "Schwerpunkt/Controllers/FlipperPlayerController.h"

bool UEscapeMenuWidget::Initialize()
{
	Super::Initialize();

	if (RematchButton)
	{
		RematchButton->OnClicked.AddDynamic(this, &UEscapeMenuWidget::OnRematchButtonClicked);
	}

	FlipperPlayerController = Cast<AFlipperPlayerController>(GetOwningPlayer());
	
	return true;
}

void UEscapeMenuWidget::OnRematchButtonClicked()
{
	if (RematchButton)
	{
		RematchButton->SetIsEnabled(false);
	}

	if (FlipperPlayerController)
	{
		FlipperPlayerController->ServerRequestRematch();
	}
}
