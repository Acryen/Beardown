// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperObstacleBase.h"
#include "FlipperArmObstacle.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperArmObstacle : public AFlipperObstacleBase
{
	GENERATED_BODY()
	
public:	
	AFlipperArmObstacle();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	/*
	 * Arm Rotation
	 */
	
	FRotator StartRotation;
	FRotator TargetRotation;
	void RotateArmForward(float DeltaTime);
	void RotateArmBackward(float DeltaTime);
	
	UPROPERTY(EditAnywhere)
	float RotationAngle = 45.f;
	UPROPERTY(EditAnywhere)
	float RotationSpeedForward = 10.f;
	UPROPERTY(EditAnywhere)
	float RotationSpeedBackward = 20.f;

public:
	FORCEINLINE virtual void StartMovement() override { MovementState = EMovementState::EMS_RotateForward; }
};
