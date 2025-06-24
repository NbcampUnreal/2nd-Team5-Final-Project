// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectRotationStatue.h"
#include "StillLoading\Character\SLPlayerCharacter.h"
#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Minigame/System/SLMiniGameUtility.h"

ASLInteractableObjectRotationStatue::ASLInteractableObjectRotationStatue()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	RotationStates = {};
}

void ASLInteractableObjectRotationStatue::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);
	if (RotationStates.Num() == 0 || bRotateTrigger)
	{
		return;
	}
	
	CurrentRotationIndex = (CurrentRotationIndex + 1) % RotationStates.Num();
	//PuzzleManager->TryCount++;
	SetRotate(RotationStates[CurrentRotationIndex]);

}

void ASLInteractableObjectRotationStatue::BeginPlay()
{
	Super::BeginPlay();

	ASLMinigamePuzzleCond* PuzzleCond = FindActorInWorld<ASLMinigamePuzzleCond>(GetWorld());

	if (PuzzleCond)
	{
		PuzzleManager = PuzzleCond;
		UE_LOG(LogTemp, Warning, TEXT("Find PuzzleCond Succeed"));
		PuzzleManager->ObjectResetRequested.AddDynamic(this, &ASLInteractableObjectRotationStatue::ResetCondition);
	}


	InitialRotation = GetActorRotation();
	TargetRotation = FRotator::ZeroRotator;
}

void ASLInteractableObjectRotationStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotateTrigger)
	{
		RotateActor(DeltaTime);

	}
}

void ASLInteractableObjectRotationStatue::SetRotate(FRotator TargetRotator)
{
	//임시
	StartRotation = GetActorRotation();
	TargetRotation.Yaw = TargetRotator.Yaw;
	bRotateTrigger = true;
	SetActorTickEnabled(true);
}

void ASLInteractableObjectRotationStatue::ResetCondition()
{
	SetRotate(RotationStates[0]);
	CurrentRotationIndex = 0;
	//PuzzleManager->TryCount = 0;//임시
	ResetTrigger = true;
	UE_LOG(LogTemp, Warning, TEXT("Reset Condition"));

}

void ASLInteractableObjectRotationStatue::RotateActor(float DeltaTime)
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
		
		if (PuzzleManager)
		{
			PuzzleManager->UpdateStatueState(StatueIndex, CurrentRotationIndex);
		}
		
	}
}
