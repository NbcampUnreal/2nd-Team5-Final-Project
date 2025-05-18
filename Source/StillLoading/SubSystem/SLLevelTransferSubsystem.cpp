// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SubSystem/SLLevelTransferSettings.h"
#include "SubSystem/DataAssets/SLLevelDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"

void USLLevelTransferSubsystem::OpenLevelByNameType(ESLLevelNameType LevelNameType, const FString& Option)
{
	CheckValidOfLevelDataAsset();

	CurrentLevel = LevelNameType;
	FString LevelURL = LevelDataAsset->GetLevelRef(LevelNameType).GetAssetName();
	UGameplayStatics::OpenLevel(GetWorld(), (FName)LevelURL, true, Option);
}

const ESLLevelNameType USLLevelTransferSubsystem::GetCurrentLevelType() const
{
	return CurrentLevel;
}

void USLLevelTransferSubsystem::SetCurrentChapter(ESLChapterType ChapterType)
{
	CurrentChapter = ChapterType;

	CheckValidOfUISubsystem();
	UISubsystem->SetChapterToUI(ChapterType);

	ChapterDelegate.Broadcast(ChapterType);
}

const ESLChapterType USLLevelTransferSubsystem::GetCurrentChapter() const
{
	return CurrentChapter;
}

void USLLevelTransferSubsystem::CheckValidOfLevelDataAsset()
{
	CheckValidOfLevelTransferSettings();

	if (IsValid(LevelDataAsset) &&
		DataChapter == CurrentChapter)
	{
		return;
	}

	DataChapter = CurrentChapter;
	LevelDataAsset = LevelSettings->ChapterLevelDataMap[CurrentChapter].LoadSynchronous();
	checkf(IsValid(LevelDataAsset), TEXT("Level Data Asset Load fail"));
}

void USLLevelTransferSubsystem::CheckValidOfLevelTransferSettings()
{
	if (IsValid(LevelSettings))
	{
		return;
	}

	LevelSettings = GetDefault<USLLevelTransferSettings>();
	checkf(IsValid(LevelSettings), TEXT("Level Transfer Settings is invalid"));
}

void USLLevelTransferSubsystem::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return;
	}

	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	checkf(IsValid(UISubsystem), TEXT("UISubsystem is invalid"));
}
