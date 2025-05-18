// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRotationStatue.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"

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
			}
		},
		0.1f,
		true);
}

void ASLReactiveObjectRotationStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
