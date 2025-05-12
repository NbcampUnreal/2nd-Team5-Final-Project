// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLIntroWidget.generated.h"


UCLASS()
class STILLLOADING_API USLIntroWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;

protected:
	virtual void OnEndedOpenAnim() override;
	virtual void OnEndedCloseAnim() override;

private:
	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> IntroOpenAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> IntroCloseAnim = nullptr;
};
