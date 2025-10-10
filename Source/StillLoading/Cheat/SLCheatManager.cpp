// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCheatManager.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Interaction/SLInteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameMode/SLGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveBase.h"
#include "Render/SLGridVolume.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "SubSystem/SLDemoSubsystem.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "UI/SLUISubsystem.h"
#include "UI/HUD/SLTitleHUD.h"

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

void USLCheatManager::SetCurrentChapter(const int32 InChapter)
{
	switch (InChapter)
	{
		default:
			break;
		case 0:
			GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>()->SetCurrentChapter(ESLChapterType::EC_Chapter0);
			break;
		case 1:
			GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>()->SetCurrentChapter(ESLChapterType::EC_Chapter1);
			break;
		case 2:
			GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>()->SetCurrentChapter(ESLChapterType::EC_Chapter2);
			break;
		case 3:
			GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>()->SetCurrentChapter(ESLChapterType::EC_Chapter3);
			break;
		case 4:
			GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>()->SetCurrentChapter(ESLChapterType::EC_Chapter4);
			break;
	}
	// GetWorld()->GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>()->SaveGameData();
	// GetWorld()->GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>()->LoadGameData();
}

void USLCheatManager::ShowTitleHUD(bool bFlag)
{
	ASLTitleHUD* HUD = Cast<ASLTitleHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD && !bFlag)
	{
		HUD->HideTitleWidget();
	}
	else if(HUD && bFlag)
	{
		HUD->NotifyTalkEnded();
	}
}

void USLCheatManager::ShowLevelWidget(bool bFlag)
{
	ASLBaseHUD* HUD = Cast<ASLBaseHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (IsValid(HUD))
	{
		HUD->HideLevelWidget(!bFlag);
	}
}

void USLCheatManager::SkipChapter()
{
	USLLevelTransferSubsystem* LevelTransferSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	check(LevelTransferSubsystem);

	switch (ESLChapterType CurrentChapter = LevelTransferSubsystem->GetCurrentChapter())
	{
	default:
		break;
	case ESLChapterType::EC_Chapter0:
		LevelTransferSubsystem->SetCurrentChapter(ESLChapterType::EC_Chapter1);
		LevelTransferSubsystem->OpenLevelByNameType(ESLLevelNameType::ELN_DebugRoom);
		break;
	case ESLChapterType::EC_Chapter1:
		LevelTransferSubsystem->SetCurrentChapter(ESLChapterType::EC_Chapter2);
		LevelTransferSubsystem->OpenLevelByNameType(ESLLevelNameType::ELN_Mini6);
		break;
	case ESLChapterType::EC_Chapter2:
		LevelTransferSubsystem->SetCurrentChapter(ESLChapterType::EC_Chapter3);
		LevelTransferSubsystem->OpenLevelByNameType(ESLLevelNameType::ELN_Mini1);
		break;
	case ESLChapterType::EC_Chapter3:
		LevelTransferSubsystem->SetCurrentChapter(ESLChapterType::EC_Chapter4);
		LevelTransferSubsystem->OpenLevelByNameType(ESLLevelNameType::ELN_BossStage);
		break;
	case ESLChapterType::EC_Chapter4:
		USLDemoSubsystem* DemoSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
		USLUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLUISubsystem>();
		DemoSubsystem->EndCurrentGame();
		UISubsystem->AddAdditiveWidget(ESLAdditiveWidgetType::EAW_CreditWidget);
		break;
	}
}
