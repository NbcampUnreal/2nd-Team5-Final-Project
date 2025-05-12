// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLMapListWidget.generated.h"


UCLASS()
class STILLLOADING_API USLMapListWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

private:

private:

};
