// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlipperBallSpawnPoint.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperBallSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlipperBallSpawnPoint();

protected:
	virtual void BeginPlay() override;

};
