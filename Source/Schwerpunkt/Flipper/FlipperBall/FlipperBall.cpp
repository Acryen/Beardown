

#include "FlipperBall.h"

#include "Kismet/GameplayStatics.h"

AFlipperBall::AFlipperBall()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	SetRootComponent(BallMesh);
	BallMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BallMesh->SetEnableGravity(true);
	BallMesh->SetSimulatePhysics(true);

	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}


void AFlipperBall::BeginPlay()
{
	Super::BeginPlay();

	if (YellowDissolveMaterialInstance)
		YellowDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(YellowDissolveMaterialInstance, this);

	if (PinkDissolveMaterialInstance)
		PinkDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(PinkDissolveMaterialInstance, this);
}

void AFlipperBall::ReactToHit(AActor* HitInstigator)
{
	if (HitBlacklistArray.Contains(HitInstigator)) return;
	
	FVector TargetDirection = (GetActorLocation() - HitInstigator->GetActorLocation()).GetSafeNormal();
	FVector TargetLocation = GetActorLocation() + TargetDirection * TravelDistanceMultiplier;
	BallMesh->AddImpulse(TargetLocation);

	HitBlacklistArray.Add(HitInstigator);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperBall::RemoveFromHitBlacklist, 0.5);
}

void AFlipperBall::RemoveFromHitBlacklist()
{
	if (HitBlacklistArray.Num() == 0) return;
	
	HitBlacklistArray.RemoveAt(0);
}

void AFlipperBall::MulticastPlayHitSound_Implementation()
{
	if (HitBallSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitBallSound, GetActorLocation(), GetActorRotation());
	}
}

// Called by the StageManager when activating Stage Three.
void AFlipperBall::MulticastStartDissolve_Implementation()
{
	BallMesh->SetMaterial(0, YellowDissolveDynamicMaterialInstance);

	DissolveTrackDelegate.BindDynamic(this, &AFlipperBall::UpdateDissolveMaterial);

	if (DissolveCurve && DissolveTimelineComponent)
	{
		DissolveTimelineComponent->AddInterpFloat(DissolveCurve, DissolveTrackDelegate);
		DissolveTimelineComponent->Play();
	}
}

void AFlipperBall::UpdateDissolveMaterial(float DissolveValue)
{
	if (bUpdateYellowDissolve)
	{
		YellowDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
	else
	{
		PinkDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AFlipperBall::MulticastReverseDissolve_Implementation()
{
	BallMesh->SetMaterial(0, PinkDissolveDynamicMaterialInstance);
	bUpdateYellowDissolve = false;
	DissolveTimelineComponent->Reverse();
}