// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Schwerpunkt/Flipper/FlipperCustomTypes/MovementState.h"
#include "FlipperArmBase.generated.h"


UCLASS()
class SCHWERPUNKT_API AFlipperArmBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlipperArmBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetEnableGlow(bool bGlowStatus);
	void StartDissolve();
	

protected:
	virtual void BeginPlay() override;

	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimelineComponent;

	/*
	 * Materials & Dissolve
	 */
	
	void SetupMaterialInstances();
	UPROPERTY()
	UMaterialInstanceDynamic* GlowDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* ColoredWoodDissolveDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* VinesDIssolveDynamicMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* SimpleWoodDissolvDynamiceMaterialInstance;
	FOnTimelineFloat DissolveTrackDelegate;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void DisableCastShadows();

	//
	// Materials & Dissolve - Editables
	
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	UMaterialInterface* ColoredWoodDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	UCurveFloat* DissolveCurve;
	UPROPERTY(EditAnywhere, Category = MaterialsAndDissolve)
	float CastShadowDelay = 1.5f;

	/*
	 * Miscellaneous
	 */
	
	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::EMS_NoMovement;
	
public:
	FORCEINLINE virtual void ActivateArm() {/* To be implemented by Child Classes */}
	FORCEINLINE UStaticMeshComponent* GetMesh() { return Mesh; }

};
