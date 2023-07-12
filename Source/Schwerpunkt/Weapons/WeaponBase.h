// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class SCHWERPUNKT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	
protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WeaponMesh;
	
	/*
	 * Miscellaneous
	 */
	
	UPROPERTY()
	class AFlipperGameCharacter* OwningCharacter;
	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:
	FORCEINLINE void SetFlipperGameCharacter(class AFlipperGameCharacter* Character) { OwningCharacter = Character;} 
};
