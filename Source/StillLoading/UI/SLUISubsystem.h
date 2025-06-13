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
	UFUNCTION(BlueprintCallable)
	void ActivateFade(bool bIsFadeIn, bool bIsMoveLevel = false);

	UFUNCTION(BlueprintCallable)
	void ActivateNotify(ESLGameMapType MapType, const FName& NotiName);

	UFUNCTION(BlueprintCallable)
	void ActivateStory(ESLStoryType TargetStoryType, const FName& StoryName);

	UFUNCTION(BlueprintCallable)
	FSLTalkDelegateBuffer& ActivateTalk(ESLTalkTargetType TalkTargetType, FName TargetName, FName TalkName);

	UFUNCTION(BlueprintCallable)
	void AddAdditiveWidget(ESLAdditiveWidgetType WidgetType);

	void SetChapterToUI(ESLChapterType ChapterType);
	void SetLevelInputMode(ESLInputModeType InputModeType, bool bIsVisibleMouseCursor);
	void RemoveCurrentAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveAllAdditveWidget();

	UDataAsset* GetPublicImageData();
	FSLTalkDelegateBuffer& GetTalkDelegate();

private:
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

	ESLChapterType CurrentDataChapter = ESLChapterType::EC_None;
	ESLInputModeType CurrentLevelInputMode = ESLInputModeType::EIM_UIOnly;
	bool bIsVisibleLevelCursor = true;
};
