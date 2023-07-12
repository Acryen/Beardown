// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlipperBallSpeedIncrease.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperBallSpeedIncrease : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlipperBallSpeedIncrease();

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	/*
	 * Miscellaneous
	 */
	
	UPROPERTY(EditAnywhere)
	float SpeedIncrease = -1.f;
	UFUNCTION()
	void OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
