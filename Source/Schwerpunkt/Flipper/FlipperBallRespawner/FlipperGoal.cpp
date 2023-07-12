// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGoal.h"

#include "Schwerpunkt/Flipper/FlipperBall/FlipperBall.h"
#include "Schwerpunkt/GameModes/FlipperGameMode.h"


AFlipperGoal::AFlipperGoal()
{
	PrimaryActorTick.bCanEverTick = false;
}

// The parent class draws a reference to the variable FlipperBall and sets the timer for the respawn.
void AFlipperGoal::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (!Cast<AFlipperBall>(OtherActor)) return;
	
	AFlipperGameMode* FlipperGameMode = Cast<AFlipperGameMode>(GetWorld()->GetAuthGameMode());
	if (!FlipperGameMode) return;
	
	FlipperGameMode->GoalScored(bIsRightGoal);
	FlipperGameMode->SetStartStageThreeDelay(FlipperBallResetTime);

	if (!(FlipperGameMode->CheckIfPlayerHasWon() == EWhatTeamWon::EWTW_NoWinner))
		bGameOver = true;

	//MulticastPlayGoalSounds();

	// The SuperCall respawns the ball. This should only happen if bGameOver = false. Hence the SuperCall at the end.
	Super::OnTriggerBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

// void AFlipperGoal::MulticastPlayGoalSounds_Implementation()
// {
// 	if (GetNetMode() == ENetMode::NM_Client)
// 		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Jo"));
// 	
// 	if (GoalHornSound)
// 		UGameplayStatics::PlaySoundAtLocation(this, GoalHornSound, GetActorLocation(), GetActorRotation());
//
// 	FTimerHandle TimerHandleOne;
// 	GetWorldTimerManager().SetTimer(TimerHandleOne, this, &ThisClass::PlayCrowdCheerSound, 1.f);
// }
//
// void AFlipperGoal::PlayCrowdCheerSound()
// {
// 	if (CrowdCheerSound && CrowdReferencePosition)
// 		UGameplayStatics::PlaySoundAtLocation(this, CrowdCheerSound, CrowdReferencePosition->GetActorLocation(), CrowdReferencePosition->GetActorRotation());
// }