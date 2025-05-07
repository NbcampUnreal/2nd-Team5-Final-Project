// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLUITypes.generated.h"

UENUM(BlueprintType)
enum class ESLAdditiveWidgetType : uint8
{
	OptionWidget = 0,
	FadeWidget,
	StoryWidget,
	TalkWidget,
	NotifyWidget,
	CreditWidget
};

UCLASS()
class STILLLOADING_API USLUITypes : public UObject
{
	GENERATED_BODY()
	
};
