// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SLBaseWidget.h"
#include "SLLevelWidget.generated.h"

struct FSLLevelWidgetDataRow;

UCLASS()
class STILLLOADING_API USLLevelWidget : public USLBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void ActivateWidget(ESLChapterType ChapterType) override;
	virtual void SetLevelWidgetData(const FSLLevelWidgetDataRow& LevelWidgetData);

protected:
	void RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType);
};
