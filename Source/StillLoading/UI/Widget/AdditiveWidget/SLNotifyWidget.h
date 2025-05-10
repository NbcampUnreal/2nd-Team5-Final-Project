// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLNotifyWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class STILLLOADING_API USLNotifyWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

	void UpdateNotifyText(const FText& NewText);

protected:
	virtual void OnEndedOpenAnim() override;
	virtual void OnEndedCloseAnim() override;

	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NotifyText = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> VisibleNotifyAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> InvisibleNotifyAnim = nullptr;
};
