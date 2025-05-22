// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectStatue.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"
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

void ASLReactiveObjectStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{

}

void ASLReactiveObjectStatue::DeactivateStatue()
{
}


