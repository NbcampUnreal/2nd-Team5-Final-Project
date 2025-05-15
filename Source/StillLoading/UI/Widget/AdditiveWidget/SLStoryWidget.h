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
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

	void UpdateStoryState(ESLChapterType ChapterType, ESLStoryType TargetStoryType, int32 TargetIndex);

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

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
