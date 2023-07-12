// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Schwerpunkt/Schwerpunkt.h"
#include "Schwerpunkt/Character/FlipperGameCharacter.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(true);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Lever, ECollisionResponse::ECR_Overlap);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		WeaponMesh->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnMeshBeginOverlap);
	}
}

void AWeaponBase::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwningCharacter) return;
	if (OwningCharacter->bIsAttacking && OtherActor->Implements<UCombatInterface>() && OtherActor != OwningCharacter)
		Cast<ICombatInterface>(OtherActor)->ReactToHit(GetOwner());	
	
	//UE_LOG(LogTemp, Warning, TEXT("%s: %s Overlap function triggered."), GetNetMode() == NM_Client ? TEXT("CLIENT") : TEXT("SERVER"), *GetName());
}