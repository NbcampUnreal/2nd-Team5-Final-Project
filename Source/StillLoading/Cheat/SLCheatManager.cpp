// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCheatManager.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Interaction/SLInteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameMode/SLGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveBase.h"
#include "Render/SLGridVolume.h"

void USLCheatManager::ShowGridVolumeDebugLine(const bool bFlag)
{
	TArray<AActor*> GridVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLGridVolume::StaticClass(), GridVolumes);

	for (AActor* GridVolume : GridVolumes)
	{
		Cast<ASLGridVolume>(GridVolume)->SetDebugLineVisibility(bFlag);
	}
}

void USLCheatManager::AddCurrentObjectiveProgress()
{
	if (ASLGameModeBase* GameMode = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (USLObjectiveBase* Objective = GameMode->GetPrimaryInProgressObjective())
		{
			Objective->AddObjectiveProgress();
		}
	}
}

void USLCheatManager::ShowCharacterDebugLine(const bool bFlag)
{
	if (const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (USLInteractionComponent* InteractionComponent = Character->GetComponentByClass<USLInteractionComponent>()){
			InteractionComponent->SetDebugLineVisibility(bFlag);
		}

		if (UBattleComponent *BattleComponent = Character->GetComponentByClass<UBattleComponent>())
		{
			BattleComponent->bShowDebugLine = bFlag;
		}
	}
}
