// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Schwerpunkt/Flipper/FlipperCustomTypes/MovementState.h"
#include "FlipperObstacleBase.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperObstacleBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlipperObstacleBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// This function has to be multicast because timers have to be cleared from the SliderObjects.
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastStopMovement();

protected:
	virtual void BeginPlay() override;

	/*
	* Movement
	*/
	
	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::EMS_NoMovement;

	UPROPERTY(EditAnywhere)
	bool bStartActive = false;

	/*
	 * Miscellaneous
	 */
	
	UPROPERTY(EditAnywhere)
	bool bIsStageTwoObstacle = true;
	
private:
	/*
	 * References
	 */
	
	UPROPERTY(EditInstanceOnly)
	class AStageManager* StageManager;

public:
	// AStageManager wants to enable all obstacles when retrieving StageTwo. Because some only move forwards/backwards and some others
	// only rotate, it needs a general Activate() function that the obstacles themselves implement.
	FORCEINLINE virtual void StartMovement() {}
	FORCEINLINE void ActivateMoveForward() { MovementState = EMovementState::EMS_MoveForward; }
	FORCEINLINE void ActivateMoveBackward() { MovementState = EMovementState::EMS_MoveBackward; }
};
