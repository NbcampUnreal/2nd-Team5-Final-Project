// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "UI/Struct/SLInGameDelegateBuffers.h"
#include "SLUISubsystem.generated.h"

class USLAdditiveWidget;
class USLUISettings;
class UAudioComponent;
class USLStoryWidget;

UCLASS()
class STILLLOADING_API USLUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ActivateOption();

	UFUNCTION(BlueprintCallable)
	void ActivateFade(bool bIsFadeIn, bool bIsMoveLevel = false);

	UFUNCTION(BlueprintCallable)
	void ActivateNotify(ESLGameMapType MapType, const FName NotiName);

	UFUNCTION(BlueprintCallable)
	USLStoryWidget* ActivateStory(ESLStoryType TargetStoryType);

	UFUNCTION(BlueprintCallable)
	USLBaseTextPrintWidget* ActivateTalk(ESLTalkTargetType TalkTargetType, FName TargetName, FName TalkName);

	UFUNCTION(BlueprintCallable)
	void AddAdditiveWidget(ESLAdditiveWidgetType WidgetType);

	void SetChapterToUI(ESLChapterType ChapterType);
	void SetLevelInputMode(ESLInputModeType InputModeType, bool bIsVisibleMouseCursor);
	void RemoveCurrentAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void RemoveAllAdditveWidget();

	UDataAsset* GetPublicImageData();
	USLBaseTextPrintWidget* GetTalkWidget();

private:
	void SetInputModeAndCursor(bool bIsRemove);

	void CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void CheckValidOfUISettings();
	void CheckValidOfWidgetDataAsset();
	void CheckValidOfOptiondDataAsset();

private:
	UPROPERTY()
	const USLUISettings* UISettings;

	UPROPERTY()
	TMap<ESLAdditiveWidgetType, USLAdditiveWidget*> AdditiveWidgetMap;

	UPROPERTY()
	TArray<USLAdditiveWidget*> ActiveAdditiveWidgets;

	UPROPERTY()
	FSLWidgetActivateBuffer WidgetActivateBuffer;

	UPROPERTY()
	ESLChapterType CurrentDataChapter = ESLChapterType::EC_None;

	UPROPERTY()
	ESLChapterType CurrentOptionDataChapter = ESLChapterType::EC_None;

	UPROPERTY()
	ESLInputModeType CurrentLevelInputMode = ESLInputModeType::EIM_UIOnly;

	UPROPERTY()
	bool bIsVisibleLevelCursor = true;
};