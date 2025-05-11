// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SLUITypes.h"
#include "SLBaseWidget.generated.h"

class USLUISubsystem;

UCLASS()
class STILLLOADING_API USLBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType);
	virtual void ActivateWidget(ESLChapterType ChapterType);
	virtual void DeactivateWidget() {};

	virtual void ApplyOnChangedLanguage(ESLLanguageType LanguageType) {};
	void ApplyOnChangedChapter(ESLChapterType ChapterType);

protected:
	UFUNCTION()
	virtual void OnEndedOpenAnim() {};

	UFUNCTION()
	virtual void OnEndedCloseAnim();

	virtual void FindWidgetData() {};
	virtual void ApplyImageData() {};
	virtual void ApplyFontData() {};

	void PlayUISound(ESLUISoundType SoundType);

	void CheckValidOfUISubsystem();

protected:
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> OpenAnim = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> CloseAnim = nullptr;

	UPROPERTY()
	TMap<FName, UTexture2D*> ImageMap;

	UPROPERTY()
	TObjectPtr<UFont> Font = nullptr;

	ESLAdditiveWidgetType WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;
	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;

	FWidgetAnimationDynamicEvent EndOpenAnimDelegate;
	FWidgetAnimationDynamicEvent EndCloseAnimDelegate;
};
