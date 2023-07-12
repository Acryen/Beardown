// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlipperBallRespawner.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperBallRespawner : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AFlipperBallRespawner();

protected:
	virtual void BeginPlay() override;

	/*
	* Components
	*/
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	/*
     * StartingLever References
     */
	
	UPROPERTY()
	class AFlipperLever* StartingLeverOne;
	UPROPERTY()
	class AFlipperLever* StartingLeverTwo;	

	/*
	 * FlipperBall Respawn Functionality
	 */
	
	UPROPERTY()
	class AFlipperBall* FlipperBall;
	virtual void RestartFlipperBall();
	UPROPERTY()
	AActor* FlipperBallSpawnPointActor;
	bool bGameOver = false;
	
	UPROPERTY(EditAnywhere)
	float FlipperBallResetTime = 4.f;

	/*
	 * Miscellaneous
	 */
	
	UFUNCTION()
	virtual void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// The SpawnPoint Actor should set a reference to itself for all goals.
	FORCEINLINE void SetFlipperBallSpawnActor(AActor* SpawnPointActor) { FlipperBallSpawnPointActor = SpawnPointActor; }	
};
