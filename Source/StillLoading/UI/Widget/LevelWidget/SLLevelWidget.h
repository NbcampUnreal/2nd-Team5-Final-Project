// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SLBaseWidget.h"
#include "SLLevelWidget.generated.h"


UCLASS()
class STILLLOADING_API USLLevelWidget : public USLBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void ActivateWidget(ESLChapterType ChapterType) override;

protected:
	virtual void FindWidgetData() override;
	
	void RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType);
};
