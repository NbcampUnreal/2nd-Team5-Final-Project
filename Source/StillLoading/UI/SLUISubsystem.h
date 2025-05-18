// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SLUISubsystem.generated.h"

class USLAdditiveWidget;
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
	UFUNCTION(BlueprintCallable)
	void ActivateNotify(ESLGameMapType MapType, ESLNotifyType NotiType);
	UFUNCTION(BlueprintCallable)
	void ActivateStory(ESLStoryType TargetStoryType, int32 TargetIndex);
	UFUNCTION(BlueprintCallable)
	void ActivateTalk(ESLTalkTargetType TalkTargetType, int32 TargetIndex);

	UFUNCTION(BlueprintCallable)
	void AddAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveCurrentAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveAllAdditveWidget();

	UDataAsset* GetPublicImageData();

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SetInputModeAndCursor();

	void CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void CheckValidOfUISettings();
	void CheckValidOfWidgetDataAsset();

private:
	UPROPERTY()
	const USLUISettings* UISettings;

	UPROPERTY()
	TMap<ESLAdditiveWidgetType, USLAdditiveWidget*> AdditiveWidgetMap;

	UPROPERTY()
	TArray<USLAdditiveWidget*> ActiveAdditiveWidgets;

	UPROPERTY()
	FSLWidgetActivateBuffer WidgetActivateBuffer;

	ESLInputModeType CurrentLevelInputMode = ESLInputModeType::EIM_UIOnly;
	bool bIsVisibleLevelCursor = true;
};
