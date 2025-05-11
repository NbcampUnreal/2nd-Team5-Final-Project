// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectStatue.h"
#include "StillLoading\Character\SLBaseCharacter.h"
#include "StillLoading\Minigame\System\SLMinigamePuzzleCond.h"
#include "EngineUtils.h"

ASLReactiveObjectStatue::ASLReactiveObjectStatue()
{
}

void ASLReactiveObjectStatue::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ASLMinigamePuzzleCond> It(GetWorld()); It; ++It)
	{
		if (It->IsValidLowLevel())
		{
			It->RegisterStatue(this);
			break;
		}
	}
}

//void ASLReactiveObjectStatue::RotationStatue(const ASLBaseCharacter* InCharacter)
//{
//	if (RotationStates.Num() == 0) return;
//
//	CurrentRotationIndex = (CurrentRotationIndex + 1) % RotationStates.Num();
//
//	const FRotator& NextRotation = RotationStates[CurrentRotationIndex];
//
//	SetActorRotation(NextRotation);
//}
//
//void ASLReactiveObjectStatue::LuminousStatue(const ASLBaseCharacter* InCharacter)
//{
//
//}

void ASLReactiveObjectStatue::OnReacted(const ASLBaseCharacter* InCharacter)
{

}

void ASLReactiveObjectStatue::DeactivateStatue()
{
}


