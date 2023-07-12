// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperObstacleBase.h"
#include "FlipperSpinnerObstacle.generated.h"

/**
 * 
 */
UCLASS()
class SCHWERPUNKT_API AFlipperSpinnerObstacle : public AFlipperObstacleBase
{
	GENERATED_BODY()

public:
	AFlipperSpinnerObstacle();
	virtual void Tick(float DeltaSeconds) override;

	virtual void StartMovement() override;

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
	 * Movement
	 */
	
	FRotator StartRotation;
	FRotator TargetRotation;
	void RotateForward(float DeltaTime);
	void UpdateTargetRotation(bool bDoesRotateClockwise);

	UPROPERTY(EditAnywhere, Category = Movement)
	float RotationSpeed = 50.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	bool bRotateClockwise = true;
	
};
