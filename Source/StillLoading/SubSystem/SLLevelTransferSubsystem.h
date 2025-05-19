// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLLevelTransferSubsystem.generated.h"

class USLLevelTransferSettings;
class USLLevelDataAsset;
class USLUISubsystem;
class USLSoundSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedChapter, ESLChapterType, ChapterType);

UCLASS()
class STILLLOADING_API USLLevelTransferSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentChapter(ESLChapterType ChapterType);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OpenLevelByNameType(ESLLevelNameType LevelNameType, const FString& Option = ((FString)(L"")));

	const ESLLevelNameType GetCurrentLevelType() const;
	const ESLChapterType GetCurrentChapter() const;

private:
	void PostOpenLevel(UWorld* LoadedWorld);

	void CheckValidOfLevelDataAsset();
	void CheckValidOfLevelTransferSettings();
	void CheckValidOfUISubsystem();
	void CheckValidOfSoundSubsystem();

public:
	FOnChangedChapter ChapterDelegate;

private:
	UPROPERTY()
	const USLLevelTransferSettings* LevelSettings = nullptr;

	UPROPERTY()
	TObjectPtr<USLLevelDataAsset> LevelDataAsset = nullptr;

	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<USLSoundSubsystem> SoundSubsystem = nullptr;

	UPROPERTY()
	ESLLevelNameType CurrentLevel = ESLLevelNameType::ELN_None;

	UPROPERTY()
	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;

	ESLChapterType DataChapter = ESLChapterType::EC_None;
};
