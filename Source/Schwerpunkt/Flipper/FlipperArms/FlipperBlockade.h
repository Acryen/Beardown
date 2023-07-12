// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperArmBase.h"
#include "FlipperBlockade.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperBlockade : public AFlipperArmBase
{
	GENERATED_BODY()
	
public:	
	AFlipperBlockade();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Moving Functionality
	 */
	
	FVector StartLocation;
	FVector TargetLocation;
	void MoveForward(float DeltaTime);
	void MoveBackward(float DeltaTime);

	//
	// Moving Functionality - Editables
	
	UPROPERTY(EditAnywhere, Category = MovingFunctionality)
	bool bMoveForwardInsteadOfLeft = false;
	UPROPERTY(EditAnywhere, Category = MovingFunctionality)
	float MovingDistance = 800.f;
	UPROPERTY(EditAnywhere, Category = MovingFunctionality)
	float MovingSpeedForward = 300.f;
	UPROPERTY(EditAnywhere, Category = MovingFunctionality)
	float MovingSpeedBackward = 300.f;

	UPROPERTY(EditAnywhere)
	USoundBase* MovingSound;

public:
	FORCEINLINE virtual void ActivateArm() override { MovementState = EMovementState::EMS_RotateForward; }

};
