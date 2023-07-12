// Copyright Epic Games, Inc. All Rights Reserved.

#include "SchwerpunktGameMode.h"
#include "SchwerpunktCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASchwerpunktGameMode::ASchwerpunktGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
