// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerControllerBase.h"

#include "Schwerpunkt/Character/FlipperGameCharacter.h"


void ACustomPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &ThisClass::ToggleEscapeMenu);
}

void ACustomPlayerControllerBase::ServerDestroyPawnsWeapon_Implementation()
{
	AFlipperGameCharacter* PlayerCharacter = Cast<AFlipperGameCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->DestroyWeapon();
	}
}
