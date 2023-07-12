// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZActor.generated.h"

UCLASS()
class SCHWERPUNKT_API AKillZActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AKillZActor();

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	/*
	 * Miscellaneous
	 */
	
	UFUNCTION()
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
