// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Schwerpunkt/Interfaces/CombatInterface.h"
#include "FlipperBall.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperBall : public AActor, public ICombatInterface
{
	GENERATED_BODY()
	
public:	
	AFlipperBall();
	
protected:
	virtual void BeginPlay() override;

private:
	/*
     * Components
     */
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BallMesh;
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;

public:
	/*
	 * Impulse
	 */
	
	// Implemented von ICombatInterface
	virtual void ReactToHit(AActor* HitInstigator) override;

private:
	UPROPERTY()
	TArray<AActor*> HitBlacklistArray;
	void RemoveFromHitBlacklist();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayHitSound();

	//
	// Impulse - Editables
	
	UPROPERTY(EditAnywhere, Category = Impulse)
	float TravelDistanceMultiplier = 100.f;
	UPROPERTY(EditAnywhere, Category = Impulse)
	USoundBase* HitBallSound;

public:
	/*
	 * Instanced Materials & Dissolve
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartDissolve();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReverseDissolve();

private:
	UPROPERTY()
	UMaterialInstanceDynamic* YellowDissolveDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* PinkDissolveDynamicMaterialInstance;
	FOnTimelineFloat DissolveTrackDelegate;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	bool bUpdateYellowDissolve = true;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* YellowDissolveMaterialInstance;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* PinkDissolveMaterialInstance;
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

public:
	/* This function changes the LinearDamping value in the StaticMesh of the ball.
	 * Default value is 0.01. The higher the value, the higher the air resistance.
	 * Negative values increase speed.*/
	FORCEINLINE void SetSpeed(float SpeedValue) { BallMesh->SetLinearDamping(SpeedValue); }

};
