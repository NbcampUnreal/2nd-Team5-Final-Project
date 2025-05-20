// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLWidgetActivateBuffer.generated.h"

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLWidgetActivateBuffer
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;

	UPROPERTY()
	ESLGameMapType TargetMap = ESLGameMapType::EGM_None;

	UPROPERTY()
	ESLStoryType TargetStory = ESLStoryType::ES_Start;

	UPROPERTY()
	ESLTalkTargetType TargetTalk = ESLTalkTargetType::ETT_None;

	UPROPERTY()
	ESLNotifyType TargetNotify = ESLNotifyType::EN_None;

	UPROPERTY()
	int32 TargetIndex = 0;

	UPROPERTY()
	bool bIsFade = true;

	UPROPERTY()
	bool bIsMoveLevel = false;

	UPROPERTY()
	TObjectPtr<UDataAsset> WidgetPublicData = nullptr;

	UPROPERTY()
	TObjectPtr<UDataAsset> WidgetPrivateData = nullptr;
};
