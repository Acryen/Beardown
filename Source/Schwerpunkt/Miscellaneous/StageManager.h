// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageManager.generated.h"

UENUM(BlueprintType)
enum class EBoardTransitionState : uint8
{
	EBTS_NoTransition UMETA(DisplayName = "No Transition"),
	EBTS_PullStageOne UMETA(DisplayName = "Pull Stage One Back"),
	EBTS_PushStageTwo UMETA(DisplayName = "Push Stage Two"),
	EBTS_PullStartingStage UMETA(DisplayName = "Pull Starting Stage"),
	EBTS_PushStageThree UMETA(DisplayName = "Push Stage Three"),

	EBTS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SCHWERPUNKT_API AStageManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AStageManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Board Transition Functionality
	 */
	
	UPROPERTY(Replicated)
	EBoardTransitionState BoardTransitionState = EBoardTransitionState::EBTS_NoTransition;

	/* Moves all of the Geometry in an array to their TargetLocation. Returned true once all actors have reached their destination.
	 * @PARAM DeltaSeconds is used for interpolation
	 * @PARAM GeometryArray contains all actors to be moved
	 * @PARAM TargetLocationArray contains the TargetLocations of all actors to be moved
	 * @PARAM MovingSpeed determines the speed of the interpolation */
	bool MoveStageGeometry(float DeltaSeconds, TArray<AActor*>& GeometryArray, TArray<FVector>& TargetLocationArray, float MovingSpeed);

	//
	// Stage One
	
	UPROPERTY()
	TArray<AActor*> StageOneGeometryArray;
	TArray<FVector> StageOneGeometryTargetLocationArray;
	UPROPERTY()
	TArray<class AFlipperLever*> StageOneLeverArray;
	void PullStageOneBack(float DeltaSeconds);
	void MoveDownStageOneLevers();
	bool bMoveDownStageOneLeversNotCalled = true;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayPullStageOneSound();

	//
	// Stage One - Editables
	
	UPROPERTY(EditAnywhere, Category = StageOne)
	float FlipperGeometryMovingDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = StageOne)
	float FlipperGeometryMoveSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = StageOne)
	USoundBase* PullStageOneSound;
	
public:
	//
	// Stage Two
	
	void ActivateStageTwo(TArray<class AFlipperLever*> StartingFlipperLeverArray, TArray<class AFlipperLever*> StageOneFlipperLeverArray,
		TArray<class AFlipperLever*> StageTwoFlipperLeverArray,	TArray<class AFlipperLever*> KeeperFlipperLeverArray);

private:
	UPROPERTY()
	TArray<AActor*> StageTwoGeometryArray;
	TArray<FVector> StageTwoGeometryTargetLocationArray;
	UPROPERTY()
	TArray<class AFlipperObstacleBase*> StageTwoObstacleArray;
	UPROPERTY(Replicated)
	TArray<class AFlipperLever*> StageTwoLeverArray;
	void PushStageTwo(float DeltaSeconds);
	void MoveUpStageTwoLevers();
	bool bMoveUpStageTwoLeversNotCalled = true;
	bool bMoveDownStageTwoLeversNotCalled = true;
	
	void PlayPushStageTwoSound();

	//
	// Stage Two - Editables
	
	UPROPERTY(EditAnywhere, Category = StageTwo)
	USoundBase* PushStageTwoSound;

public:
	//
	// Stage Three - StopObstalces & Dissolve
	
	void ActivateStageThree(float StartingDelay);

private:
	UPROPERTY(Replicated)
	TArray<class AFlipperLever*> StartingLeverArray;
	UPROPERTY(Replicated)
	TArray<class AFlipperLever*> KeeperLeverArray;
	UPROPERTY()
	TArray<AFlipperObstacleBase*> StageThreeObstacleArray;
	void StopObstacleMovement();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDissolveStartingLevers();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDissolveStageTwoLevers();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayStageTwoBrokenSound();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayEarthquakeSound();

	//
	// Stage Three - StopObstacles & Dissolve - Editables
	
	UPROPERTY(EditAnywhere, Category = StageThreeObstaclesAndDissolve)
	float StartingLeverDissolveDelay = 3.5f;
	UPROPERTY(EditAnywhere, Category = StageThreeObstaclesAndDissolve)
	float StageTwoLeverDissolveDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = StageThreeObstaclesAndDissolve)
	float EarthquakeDelay = 1.f;
	UPROPERTY(EditInstanceOnly, Category = StageThreeObstaclesAndDissolve)
	class AFlipperBall* FlipperBall;

	UPROPERTY(EditAnywhere, Category = StageThreeObstaclesAndDissolve)
	USoundBase* StageTwoBrokenSound;
	UPROPERTY(EditAnywhere, Category = StageThreeObstaclesAndDissolve)
	USoundBase* EarthquakeSound;

	//
	// Stage Three - DisablePlayerMovement & Camera
	
	void StartCameraShake();
	void ShowExclamationMark();
	void DisablePlayerMovement();
	void StartCameraBlend();
	void StopCameraBlend();
	void EnablePlayerMovement();

	//
	// Stage Three - DisablePlayerMovement & Camera - Editables
	
	UPROPERTY(EditAnywhere, Category = StageThreeCharacterAndCamera)
	float CameraShakeDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = StageThreeCharacterAndCamera)
	float ExclamationDelay = 1.f;
	UPROPERTY(EditAnywhere, Category = StageThreeCharacterAndCamera)
	float DisableMovementDelay = 1.f;
	UPROPERTY(EditAnywhere, Category = StageThreeCharacterAndCamera)
	float CameraStartBlendDelay = 1.f;
	UPROPERTY(EditAnywhere, Category = StageThreeCharacterAndCamera)
	float CameraStopBlendDelay = 3.f;

	//
	// Stage Three - Geometry Transition
	
	void StartStageThreeGeometryTransition();
	UPROPERTY()
	TArray<AActor*> StartingStageGeometryArray;
	TArray<FVector> StartingStageGeometryTargetLocationArray;
	UPROPERTY()
	TArray<AActor*> StageThreeActorArray;
	TArray<FVector> StageThreeGeometryTargetLocationArray;
	void PullStartingStage(float DeltaSeconds);
	void PushStageThree(float DeltaSeconds);
	void ReverseBallDissolve();
	UPROPERTY()
	TArray<AActor*> StageThreeCollisionBoxesArray;
	void EnableBoarderCollisionBoxes();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayStartingStageTransitionSound();

	//
	// Stage Three - Geometry Transition - Editables
	
	UPROPERTY(EditAnywhere, Category = StageThreeGeometryTransition)
	float GroundGeometryMovingDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = StageThreeGeometryTransition)
	float GroundGeometryMovingSpeed = 100.f;
	UPROPERTY(EditAnywhere, Category = StageThreeGeometryTransition)
	float ReverseBallDissolveDelay = 3.f;

	UPROPERTY(EditAnywhere, Category = StageThreeGeometryTransition)
	USoundBase* StartingStageTransitionSound;

public: 
	/*
	 * End of Game
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStageTwoToEndTransition();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStageThreeToEndTransition();
	UPROPERTY(Replicated)
	bool bRedTeamHasWon = false;

private:
	// Enables EndStage Podium, Cage, corresponding FlipperArm/-Lever and Destroys FlipperBall
	void EnablePodiumAndCage();
	void PlayVictoryMusic();

	//
	// End of Game - Editables
	
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	class AStaticMeshActor* WinnerPodium;
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	class AStaticMeshActor* Cage;
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	class AFlipperLever* EndOfGameFlipperLever;
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	class AFlipperArmBase* EndOfGameFlipperArm;
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	UMaterialInterface* PodiumBlueMaterial;

	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	class AAmbientSound* AmbientSoundActor;
	UPROPERTY(EditInstanceOnly, Category = EndOfGame)
	USoundBase* VictoryMusic;
	
	/*
	 * Miscellaneous
     */
	
	UPROPERTY()
	class AFlipperGameMode* FlipperGameMode;
	UPROPERTY(EditInstanceOnly)
	AActor* FlipperBoard;

public:
	FORCEINLINE void AddStageTwoObstacleToArray(class AFlipperObstacleBase* FlipperObstacle) { StageTwoObstacleArray.Add(FlipperObstacle); }
	FORCEINLINE void AddStageThreeObstacleToArray(class AFlipperObstacleBase* FlipperObstacle) { StageThreeObstacleArray.Add(FlipperObstacle); }

	//
	//Debug
	
	FORCEINLINE void DebugShowExclamationMark() { ShowExclamationMark(); }
};
