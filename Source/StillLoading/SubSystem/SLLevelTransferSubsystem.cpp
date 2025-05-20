// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SubSystem/SLLevelTransferSettings.h"
#include "SubSystem/DataAssets/SLLevelDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLSoundSubsystem.h"

void USLLevelTransferSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::PostOpenLevel);

	// TODO : OpenLevelByNameType(ESLLevelNameType::Intro);
}

void USLLevelTransferSubsystem::OpenLevelByNameType(ESLLevelNameType LevelNameType, bool bIsFadeOut, const FString& Option)
{
	CurrentLevel = LevelNameType;
	OptionString = Option;

	if (!bIsFadeOut)
	{
		PostFadeOut();
		return;
	}
	
	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(false, true);
}

void USLLevelTransferSubsystem::PostFadeOut()
{
	CheckValidOfLevelDataAsset();

	if (!LevelDataAsset->GetLevelRef(CurrentLevel).IsNull())
	{
		FString LevelURL = LevelDataAsset->GetLevelRef(CurrentLevel).GetAssetName();
		UGameplayStatics::OpenLevel(GetWorld(), (FName)LevelURL, true, OptionString);
	}
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

void USLLevelTransferSubsystem::PostOpenLevel(UWorld* LoadedWorld)
{
	CheckValidOfLevelTransferSettings();
	CheckValidOfSoundSubsystem();

	if (LevelSettings->LevelBgmMap.Contains(CurrentLevel))
	{
		SoundSubsystem->PlayBgmSound(LevelSettings->LevelBgmMap[CurrentLevel]);
	}
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

void USLLevelTransferSubsystem::CheckValidOfSoundSubsystem()
{
	if (IsValid(SoundSubsystem))
	{
		return;
	}

	SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
}
