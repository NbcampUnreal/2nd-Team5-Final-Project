// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "SLStoryWidget.generated.h"

class UImage;

UCLASS()
class STILLLOADING_API USLStoryWidget : public USLBaseTextPrintWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;
	virtual bool ApplyTextBorderImage() override;
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle);

private:
	void UpdateStoryState(ESLChapterType ChapterType, ESLStoryType TargetStoryType, int32 TargetIndex);

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SkipButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> FastButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> StoryText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> StoryBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> NameBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> NamePanel = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> StoryImg = nullptr;


	ESLStoryType CurrentStoryType = ESLStoryType::ES_Start;
	int32 CurrentStoryIndex = 0;
};
