// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SLUISubsystem.generated.h"

class USLBaseWidget;
class USLUISettings;
class UAudioComponent;

UCLASS()
class STILLLOADING_API USLUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void SetChapterToUI(ESLChapterType ChapterType);
	void SetLanguageToUI(ESLLanguageType LanguageType);

	void AddAdditveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveCurrentAdditiveWidget();
	void RemoveAllAdditveWidget();

	void PlayUISound(ESLUISoundType SoundType);
	void StopUISound();
	
	const ESLChapterType GetCurrentChapter() const;
	const ESLLanguageType GetCurrentLanguage() const;

	const UDataTable* GetImageDataTable();

	//temp
	void SetEffectVolume(float VolumeValue);

private:
	bool CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	bool CheckValidOfUISettings();

	bool CheckValidOfSoundSource(ESLUISoundType SoundType);
	bool CheckValidOfImageDataTable();

private:
	UPROPERTY()
	const USLUISettings* UISettings;

	UPROPERTY()
	TMap<ESLAdditiveWidgetType, USLBaseWidget*> AdditiveWidgetMap;

	UPROPERTY()
	TMap<ESLUISoundType, USoundBase*> UISoundMap;

	UPROPERTY()
	TArray<USLBaseWidget*> ActiveAdditiveWidgets;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComp = nullptr;

	UPROPERTY()
	TObjectPtr<UDataTable> WidgetImageData = nullptr;

	UPROPERTY()
	int32 ActiveCount = 0;

	UPROPERTY()
	ESLChapterType CurrentChapter = ESLChapterType::Intro;

	UPROPERTY()
	ESLLanguageType CurrentLanguage = ESLLanguageType::Kor;

	UPROPERTY()
	float EffectVolume = 1.0f;
};
