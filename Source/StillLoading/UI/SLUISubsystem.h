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
	void SetLevelInputMode(ESLInputModeType InputModeType, bool bIsVisibleMouseCursor);

	void ActivateFade(bool bIsFadeIn);
	void ActivateNotify(const FText& NotifyText);
	//void ActivateStory();
	//void ActivateTalk();

	UFUNCTION(BlueprintCallable)
	void AddAdditveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveCurrentAdditiveWidget();
	void RemoveAllAdditveWidget();

	void PlayUISound(ESLUISoundType SoundType);
	void StopUISound();
	
	const ESLChapterType GetCurrentChapter() const; //

	const UDataTable* GetImageDataTable();

	//temp
	void SetEffectVolume(float VolumeValue);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SetInputModeAndCursor();

	void CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void CheckValidOfUISettings();

	void CheckValidOfSoundSource(ESLUISoundType SoundType);
	void CheckValidOfImageDataTable();

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

	int32 ActiveCount = 0;

	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;
	ESLInputModeType CurrentLevelInputMode = ESLInputModeType::EIM_UIOnly;

	bool bIsVisibleLevelCursor = true;
	float EffectVolume = 1.0f;
};
