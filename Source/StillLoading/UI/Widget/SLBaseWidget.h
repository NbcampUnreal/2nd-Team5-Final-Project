// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SLBaseWidget.generated.h"

class USLUISubsystem;
class USLTextPoolSubsystem;

UCLASS()
class STILLLOADING_API USLBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType);

	UFUNCTION(BlueprintCallable)
	virtual void ActivateWidget(ESLChapterType ChapterType);
	virtual void DeactivateWidget() {};

	void ApplyOnChangedChapter(ESLChapterType ChapterType);

	ESLInputModeType GetWidgetInputMode() const;
	bool GetWidgetCursorMode() const;

protected:
	UFUNCTION()
	virtual void OnEndedOpenAnim() {};

	UFUNCTION()
	virtual void OnEndedCloseAnim();

	UFUNCTION()
	void NotifyChangedLanguage();

	virtual void FindWidgetData() {};
	virtual void ApplyImageData() {};
	virtual void ApplyFontData() {};
	virtual void ApplyTextData() {};

	void PlayUISound(ESLUISoundType SoundType);

	void CheckValidOfUISubsystem();
	void CheckValidOfTextPoolSubsystem();

protected:
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<USLTextPoolSubsystem> TextPoolSubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> OpenAnim = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> CloseAnim = nullptr;

	UPROPERTY()
	TMap<FName, UTexture2D*> ImageMap;

	UPROPERTY()
	FSlateFontInfo FontInfo;

	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;
	ESLInputModeType WidgetInputMode = ESLInputModeType::EIM_UIOnly;

	bool bIsVisibleCursor = true;

	FWidgetAnimationDynamicEvent EndOpenAnimDelegate;
	FWidgetAnimationDynamicEvent EndCloseAnimDelegate;
};
