// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Schwerpunkt/Interfaces/CombatInterface.h"
#include "Schwerpunkt/Weapons/WeaponBase.h"
#include "FlipperGameCharacter.generated.h"

UCLASS()
class SCHWERPUNKT_API AFlipperGameCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AFlipperGameCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	void EnablePlayerMovement();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnablePlayerMovement();
	UFUNCTION(Server, Reliable)
	void ServerDisablePlayerMovement();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisablePlayerMovement();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisablePhysicsInteraction();
	
protected:
	virtual void BeginPlay() override;

	/*
	 * Input functions
	 */
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void PerformJump();
	void Attack();
	UFUNCTION(Server, Reliable)
	void ServerAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();

private:
	/*
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerBoxForLever;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ExclamationMarkMesh;
	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* TimelineComponent;

public:
	/*
	 * Materials
	 */
	
	void SetupClothingMaterials(bool bIsRedPlayer);
	UFUNCTION(Server, Reliable)
	void ServerSetupClothingMaterials(bool IsRedPlayer);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetupClothingMaterials(bool IsRedPlayer);
	
private:
	//
	// Materials - Editables
	
	UPROPERTY(EditAnywhere, Category = ClothingMaterial)
	UMaterialInterface* ClothingMaterialInstance;
	
public:
	/*
	 * Weapon
	 */
	
	void DestroyWeapon();
	bool bIsAttacking = false;

private:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<AWeaponBase> WeaponBaseBlueprint;
	UPROPERTY(Replicated)
	class AWeaponBase* Weapon;

	/*
	 * Animations
	 */
	
	UPROPERTY(EditAnywhere, Category = Animations)
	class UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = Animations)
	class UAnimMontage* StunnedMontage;

public:
	/*
	 * Stun & Impact functionality
	 */
	virtual void ReactToHit(AActor* HitInstigator) override;

private:
	//
	// Impact functionality
	
	UPROPERTY()
	TArray<AActor*> HitBlacklistArray;
	void RemoveFromHitBlacklist();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayStunMontage();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayHitSound();

	//
	// Impact functionality - Editables
	
	UPROPERTY(EditAnywhere, Category = ImpactFunctionality)
	float CharacterLaunchVelocity = 700.f;
	UPROPERTY(EditAnywhere, Category = ImpactFunctionality)
	float CharacterLaunchZCorrection = 600.f;
	UPROPERTY(EditAnywhere, Category = ImpactFunctionality)
	float AnimaDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = ImpactFunctionality)
	USoundBase* GettingHitSound;
	
	//
	// Stun functionality
	
	UPROPERTY(ReplicatedUsing = OnRep_bIsStunned)
	bool bIsStunned = false;
	UFUNCTION()
	void OnRep_bIsStunned();
	void PerformStun();
	void ReleaseStun();

	//
	// Stun functionality - Editables
	
	UPROPERTY(EditAnywhere)
	float StunDuration = 2.f;

	/*
	 * FlipperLever and FlipperArm Glow / Highlighting Functionality
	 */
	
	UFUNCTION()
	void OnTriggerBoxForLeverBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerBoxForLeverEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(Client, Reliable)
	void ClientSetEnableHighlightingBeam(class AFlipperLever* FlipperLever, bool bVisibilityStatus);
	UFUNCTION(Client, Reliable)
	void ClientSetEnableGlow(class AFlipperLever* FlipperLever, bool bGlowStatus);

public:
	/*
	 * Exclamation Mark
	 */
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnableExclamation();

private:
	FOnTimelineFloat ExclamationTimelineDelegate;
	UFUNCTION()
	void UpdateZScale(float TimelineValue);
	void HideExclamationMark();

	//
	// Exclamation Mark - Editables
	
	UPROPERTY(EditAnywhere, Category = ExclamationMark)
	USoundBase* ExclamationMarkSound;
	UPROPERTY(EditAnywhere, Category = ExclamationMark)
	UCurveFloat* ZScaleCurve;
	UPROPERTY(EditAnywhere, Category = ExclamationMark)
	float ExclamationMarkDuration = 4.f;
	
	/*
	 * Miscellaneous
	 */
	
	UPROPERTY(EditDefaultsOnly)
	bool bIsLobbyCharacter = false;

	/*
	 * Debug
	 */
	
	UFUNCTION(Exec)
	void InitiateStageThree();

public:
	FORCEINLINE bool GetIsLobbyCharacter() { return bIsLobbyCharacter; }
};
