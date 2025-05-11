// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRotationStatue.h"
#include "StillLoading\Character\SLBaseCharacter.h"

ASLReactiveObjectRotationStatue::ASLReactiveObjectRotationStatue()
{
	RotationStates = {};
}

void ASLReactiveObjectRotationStatue::OnReacted(const ASLBaseCharacter* InCharacter)
{
	if (RotationStates.Num() == 0) return;
	
	CurrentRotationIndex = (CurrentRotationIndex + 1) % RotationStates.Num();
	
	const FRotator& NextRotation = RotationStates[CurrentRotationIndex];
	
	SetActorRotation(NextRotation);
}
