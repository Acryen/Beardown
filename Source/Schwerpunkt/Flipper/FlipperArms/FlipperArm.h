// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperArmBase.h"
#include "FlipperArm.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperArm : public AFlipperArmBase
{
	GENERATED_BODY()
	
public:	
	AFlipperArm();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;

	/*
	 * Arm Rotate Functionality
	 */
	
	FRotator StartRotation;
	FRotator TargetRotation;
	bool bGotActivated = false;
	void RotateArmForward(float DeltaTime);
	bool bResetActive = false;
	void RotateArmBackward(float DeltaTime);

public:
	virtual void ActivateArm() override;
	
private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayRotateSound();
	
	//
	// Arm Rotate Functionality - Editables
	
	UPROPERTY(EditAnywhere, Category = ArmRotateFunctionality)
	float RotationAngle = 45.f;
	UPROPERTY(EditAnywhere, Category = ArmRotateFunctionality)
	float RotationSpeedForward = 10.f;
	UPROPERTY(EditAnywhere, Category = ArmRotateFunctionality)
	float RotationSpeedBackward = 20.f;
	// RotationTargetEuqalTolerance is only used for the FlipperArm in Stage2.
	// Since no ConstInterpolation is used (because otherwise the ball behaves like shit when it's hit) we want to sort of cut out the EaseOut.
	UPROPERTY(EditAnywhere, Category = ArmRotateFunctionality)
	float RotationTargetEqualTolerance = 1.f;
	
	UPROPERTY(EditAnywhere, Category = ArmRotateFunctionality)
	USoundBase* RotateSound;

	/*
	 * Moving Down Functionality
	 */
	
	FVector StartLocation;
	FVector TargetLocation;
	bool bMovingDownActive = false;
	void MoveDown(float DeltaTime);

	//
	// Moving Down Functionality - Editables
	
	UPROPERTY(EditInstanceOnly, Category = ArmMovingDownFunctionality)
	bool bMoveDownAfterUse = false;
	UPROPERTY(EditAnywhere, Category = ArmMovingDownFunctionality)
	float MovingDownDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = ArmMovingDownFunctionality)
	float MovingDownSpeed = 100.f;

	/*
	 * Moving Up Functionality
	 */
	
	bool bMovingUpActive = false;
	void MoveUp(float DeltaTime);

public:
	FORCEINLINE void ActivateMovingDown() { MovementState = EMovementState::EMS_MoveDown; }
	FORCEINLINE void ActivateMovingUp() { MovementState = EMovementState::EMS_MoveUp; }
};
