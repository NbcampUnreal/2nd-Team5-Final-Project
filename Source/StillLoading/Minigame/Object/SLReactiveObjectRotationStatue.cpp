// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRotationStatue.h"
#include "StillLoading\Character\SLPlayerCharacter.h"
#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Minigame/System/SLMiniGameUtility.h"

ASLReactiveObjectRotationStatue::ASLReactiveObjectRotationStatue()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	RotationStates = {};
}

void ASLReactiveObjectRotationStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (RotationStates.Num() == 0 || bRotateTrigger)
	{
		return;
	}
	
	CurrentRotationIndex = (CurrentRotationIndex + 1) % RotationStates.Num();
	//PuzzleManager->TryCount++;
	SetRotate(RotationStates[CurrentRotationIndex]);

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
	PuzzleManager->ObjectResetRequested.AddDynamic(this, &ASLReactiveObjectRotationStatue::ResetCondition);

	InitialRotation = GetActorRotation();
	TargetRotation = FRotator::ZeroRotator;
}

void ASLReactiveObjectRotationStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotateTrigger)
	{
		RotateActor(DeltaTime);

	}
}

void ASLReactiveObjectRotationStatue::SetRotate(FRotator TargetRotator)
{
	//임시
	StartRotation = GetActorRotation();
	TargetRotation.Yaw = TargetRotator.Yaw;
	bRotateTrigger = true;
	SetActorTickEnabled(true);
}

void ASLReactiveObjectRotationStatue::ResetCondition()
{
	SetRotate(RotationStates[0]);
	CurrentRotationIndex = 0;
	//PuzzleManager->TryCount = 0;//임시
	ResetTrigger = true;
	UE_LOG(LogTemp, Warning, TEXT("Reset Condition"));

}

void ASLReactiveObjectRotationStatue::RotateActor(float DeltaTime)
{

	RotationElapsed += DeltaTime;

	float Alpha = FMath::Clamp(RotationElapsed / RotationDuration, 0.0f, 1.0f);

	FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
	SetActorRotation(NewRotation);
	if (Alpha >= 1.0f)
	{
		RotationElapsed = 0.0f;
		bRotateTrigger = false;
		SetActorTickEnabled(false);

		if (ResetTrigger)
		{
			ResetTrigger = false;
			return;
		}
		
		PuzzleManager->UpdateStatueState(StatueIndex, CurrentRotationIndex);
	}
}
