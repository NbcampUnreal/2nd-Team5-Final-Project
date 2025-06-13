// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectStatue.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"
#include "StillLoading\Minigame\System\SLMinigamePuzzleCond.h"
#include "EngineUtils.h"

ASLInteractableObjectStatue::ASLInteractableObjectStatue()
{
}

void ASLInteractableObjectStatue::BeginPlay()
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

void ASLInteractableObjectStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{

}

void ASLInteractableObjectStatue::DeactivateStatue()
{
}


