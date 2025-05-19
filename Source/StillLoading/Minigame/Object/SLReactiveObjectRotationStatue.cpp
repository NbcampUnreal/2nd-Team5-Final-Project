// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRotationStatue.h"
#include "StillLoading\Character\SLPlayerCharacter.h"
#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Minigame/System/SLMiniGameUtility.h"

ASLReactiveObjectRotationStatue::ASLReactiveObjectRotationStatue()
{
	PrimaryActorTick.bCanEverTick = true;
	RotationStates = {};
}

void ASLReactiveObjectRotationStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (RotationStates.Num() == 0) return;
	
	CurrentRotationIndex = (CurrentRotationIndex + 1) % RotationStates.Num();
	
	const FRotator& NextRotation = RotationStates[CurrentRotationIndex];
	GetWorldTimerManager().SetTimer(
		RotationHandle,
		[this, NextRotation]
		{
			FRotator MyRot = GetActorRotation();
			float LerpRotZ = FMath::FInterpConstantTo(MyRot.Yaw, NextRotation.Yaw, GetWorld()->GetDeltaSeconds(), LerpSpeed);
			MyRot.Yaw = LerpRotZ;
			SetActorRotation(MyRot);
			if (FMath::IsNearlyEqual(MyRot.Yaw, NextRotation.Yaw, KINDA_SMALL_NUMBER))
			{
				GetWorld()->GetTimerManager().ClearTimer(RotationHandle);
				PuzzleManager->UpdateStatueState(StatueIndex, CurrentRotationIndex);
			}
		},
		0.1f,
		true);
}

void ASLReactiveObjectRotationStatue::BeginPlay()
{
	Super::BeginPlay();

	ASLMinigamePuzzleCond* PuzzleCond = FindActorInWorld<ASLMinigamePuzzleCond>(GetWorld());

	if (PuzzleCond)
	{
		PuzzleManager = PuzzleCond;
		UE_LOG(LogTemp, Warning, TEXT("Find PuzzleCond Succeed"));
	}
}

void ASLReactiveObjectRotationStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
