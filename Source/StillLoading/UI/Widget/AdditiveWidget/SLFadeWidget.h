// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLFadeWidget.generated.h"

class UImage;

UCLASS()
class STILLLOADING_API USLFadeWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;

	void SetIsFadeIn(bool FadeValue);

protected:
	virtual void OnEndedOpenAnim() override;
	virtual void OnEndedCloseAnim() override;
	virtual void ApplyImageData() override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> FadeImage = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeInAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnim = nullptr;

	bool bIsFadeIn = true;

	static const FName FadeImgName;
};
