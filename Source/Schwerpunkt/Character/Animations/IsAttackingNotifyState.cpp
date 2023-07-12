

#include "IsAttackingNotifyState.h"

#include "Schwerpunkt/Character/FlipperGameCharacter.h"

void UIsAttackingNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(MeshComp->GetOwner());
		if (FlipperGameCharacter)
		{
			FlipperGameCharacter->bIsAttacking = true;
		}
	}
}

void UIsAttackingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AFlipperGameCharacter* FlipperGameCharacter = Cast<AFlipperGameCharacter>(MeshComp->GetOwner());
		if (FlipperGameCharacter)
		{
			FlipperGameCharacter->bIsAttacking = false;
		}
	}
}
