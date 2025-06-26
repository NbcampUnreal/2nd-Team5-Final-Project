// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCheatManager.h"

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
