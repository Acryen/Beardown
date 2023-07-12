// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Schwerpunkt/Flipper/FlipperObstacles/FlipperObstacleBase.h"
#include "FlipperSliderObstacle.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperSliderObstacle : public AFlipperObstacleBase
{
	GENERATED_BODY()
	
public:	
	AFlipperSliderObstacle();
	virtual void Tick(float DeltaTime) override;

	virtual void StartMovement() override;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* MovementTarget;

	/*
	 * Movement
	 */
	
	FVector StartLocation;
	FVector TargetLocation;
	void MoveForward(float DeltaTime);
	void MoveBackward(float DeltaTime);
	
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.f;
	UPROPERTY(EditAnywhere)
	float MovePauseDuration = 2.f;

};
