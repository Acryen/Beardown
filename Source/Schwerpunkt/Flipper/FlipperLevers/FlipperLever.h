// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Schwerpunkt/Flipper/FlipperCustomTypes/MovementState.h"
#include "Schwerpunkt/Interfaces/CombatInterface.h"
#include "FlipperLever.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperLever : public AActor, public ICombatInterface
{
	GENERATED_BODY()
	
public:	
	AFlipperLever();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeverMesh;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HighlightingLaserBeam;
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;

public:
	/*
	 * References
	 */
	
	UPROPERTY(EditInstanceOnly)
	class AFlipperArmBase* CorrespondingArm;
	
	/*
	 * Getting Hit Functionality
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
	// Getting Hit Functionality - Editables
	
	UPROPERTY(EditAnywhere)
	USoundBase* GettingHitSound;
	
private:
	
	/*
	 * Moving Down Functionality
	 */
	
	FVector StartLocation;
	FVector TargetLocation;
	void MoveDown(float DeltaTime);

public:
	void MoveDownOtherStartingLever();

private:
	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::EMS_NoMovement;

	//
	// Moving Down Functionality - Editables
	
	UPROPERTY(EditInstanceOnly, Category = MovingDownFunctionality)
	bool bMoveDownAfterUse = false;
	UPROPERTY(EditAnywhere, Category = MovingDownFunctionality)
	float MovingDownDistance = 200.f;
	UPROPERTY(EditAnywhere, Category = MovingDownFunctionality)
	float MovingDownSpeed = 80.f;

	/*
	 * Destroy StartingTriggerBox
	 */
	
	UPROPERTY(EditInstanceOnly)
	AActor* StartingTriggerBox;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeactivateStartingTriggerBox();

	/*
	 * Moving Up Functionality
	 */
	
	void MoveUp(float DeltaTime);

public:
	void MoveUpWithCorrespondingArm();

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastActivateStartingTriggerBox();

public:
	/*
	 * Highlighting Feature
	 */
	
	//
	// Highlighting LaserBeam
	void SetEnableHighlightingBeam(bool bVisibleStatus);

private:
	//
	// Highlighting LaserBeam - Editables
	
	UPROPERTY(EditAnywhere, Category = HighlightingLaserBeam)
	float HighlightingLaserBeamDistance = 280.f;
	UPROPERTY(EditAnywhere, Category = HighlightingLaserBeam)
	UMaterialInterface* HighlightingLaserBeamMaterial;
	
public:
	//
	// Materials && Dissolve
	
	void SetEnableGlow(bool bGlowStatus);

private:
	void SetupMaterialInstances();
	UPROPERTY()
	UMaterialInstanceDynamic* ButtonGlowDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* WoodDissolveDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* MetalDissolveDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* ButtonDissolveDynamicMaterialInstance;
	FOnTimelineFloat DissolveTrackDelegate;

public:
	void StartDissolve();

private:
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void DisableCastShadows();

	//
	// Materials & Dissolve - Editables
	
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	UMaterialInterface* ButtonDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	UCurveFloat* DissolveCurve;
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	float CastShadowDelay = 1.5f;
	
public:
	FORCEINLINE void ActivateMovingDown() { MovementState = EMovementState::EMS_MoveDown; }
	FORCEINLINE void ActivateMovingUp() { MovementState = EMovementState::EMS_MoveUp; }
	FORCEINLINE UStaticMeshComponent* GetMesh() { return LeverMesh; }
};
