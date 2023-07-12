// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperBallRespawner.h"
#include "FlipperGoal.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperGoal : public AFlipperBallRespawner     // <- Die Klasse erbt nicht direkt von AActor!!!!
{
	GENERATED_BODY()
	
public:	
	AFlipperGoal();

protected:
	/*
	 * Miscellaneous
	 */
	
	virtual void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
	/* TODO
	 * The MulticastPlayGoalSounds() function is not being called on the client for some reason and I couldn't understand why until the end.
	 * Functionality outsourced to BP_FlipperGoal_Asset.*/
	
	// UFUNCTION(NetMulticast, Reliable)
	// void MulticastPlayGoalSounds();
	// void PlayCrowdCheerSound();
	
private:
	UPROPERTY(EditInstanceOnly)
	bool bIsRightGoal = true;
	
	// UPROPERTY(EditInstanceOnly)
	// AActor* CrowdReferencePosition;
	//
	// UPROPERTY(EditAnywhere)
	// USoundBase* GoalHornSound;
	// UPROPERTY(EditAnywhere)
	// USoundBase* CrowdCheerSound;
	
};
