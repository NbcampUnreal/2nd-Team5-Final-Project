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
	USLStoryWidget* ActivateStory(ESLStoryType TargetStoryType, const FName& StoryName);

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

	// Test
	UFUNCTION(BlueprintCallable)
	void OnPlayerHpChanged();

	UFUNCTION(BlueprintCallable)
	void OnPlayerSpecialChanged();

	UFUNCTION(BlueprintCallable)
	void OnBossHpChanged();

private:
	void SetInputModeAndCursor();

	void CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType);
	void CheckValidOfUISettings();
	void CheckValidOfWidgetDataAsset();
	void CheckValidOfOptiondDataAsset();

	// Test
	void DecreasePlayerHp();
	void DecreaseBossHp();
	void IncreaseSpecialValue();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSLPlayerHpDelegateBuffer PlayerHpBuffer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSLSpecialValueDelegateBuffer SpecialValueBuffer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSLBossHpDelegateBuffer BossHpBuffer;

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

	// Test
	FTimerHandle PlayerHpTimer;
	FTimerHandle SpecialValueTimer;
	FTimerHandle BossHpTimer;

	float PlayerMaxHp = 100;
	float PlayerCurrentHp = 100;
	float SpecialMaxValue = 100;
	float SpecialCurrentValue = 0;
	float BossMaxHp = 100;
	float BossCurrentHp = 100;
};
