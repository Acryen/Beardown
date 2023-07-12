// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperArmBase.h"

#include "Net/UnrealNetwork.h"

AFlipperArmBase::AFlipperArmBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	DissolveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void AFlipperArmBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlipperArmBase, MovementState);
}

void AFlipperArmBase::BeginPlay()
{
	Super::BeginPlay();

	SetupMaterialInstances();
}

void AFlipperArmBase::SetupMaterialInstances()
{
	// For the glowing / highlighting feature of the Flipperarms
	GlowDynamicMaterialInstance = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	if (!GlowDynamicMaterialInstance) return;
	Mesh->SetMaterial(0, GlowDynamicMaterialInstance);

	// For the dissolv feature of the FlipperArms
	if (!ColoredWoodDissolveMaterialInstance) return;
	ColoredWoodDissolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(ColoredWoodDissolveMaterialInstance, this);
	VinesDIssolveDynamicMaterialInstance = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
	SimpleWoodDissolvDynamiceMaterialInstance = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(2), this);
}

void AFlipperArmBase::SetEnableGlow(bool bGlowStatus)
{
	if (bGlowStatus)
	{
		GlowDynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 30.f);
	}
	else
	{
		GlowDynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 1.f);
	}
}

void AFlipperArmBase::StartDissolve()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFlipperArmBase::DisableCastShadows, CastShadowDelay);

	if (!ColoredWoodDissolveDynamicMaterialInstance || !VinesDIssolveDynamicMaterialInstance || !SimpleWoodDissolvDynamiceMaterialInstance) return;
	Mesh->SetMaterial(0, ColoredWoodDissolveDynamicMaterialInstance);
	Mesh->SetMaterial(1, VinesDIssolveDynamicMaterialInstance);
	Mesh->SetMaterial(2, SimpleWoodDissolvDynamiceMaterialInstance);

	DissolveTrackDelegate.BindDynamic(this, &AFlipperArmBase::UpdateDissolveMaterial);

	if (DissolveCurve && DissolveTimelineComponent)
	{
		DissolveTimelineComponent->AddInterpFloat(DissolveCurve, DissolveTrackDelegate);
		DissolveTimelineComponent->Play();
	}
}

void AFlipperArmBase::UpdateDissolveMaterial(float DissolveValue)
{
	if (!ColoredWoodDissolveDynamicMaterialInstance || !VinesDIssolveDynamicMaterialInstance || !SimpleWoodDissolvDynamiceMaterialInstance) return;
	ColoredWoodDissolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	VinesDIssolveDynamicMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	SimpleWoodDissolvDynamiceMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
}

void AFlipperArmBase::DisableCastShadows()
{
	Mesh->SetCastShadow(false);
}
