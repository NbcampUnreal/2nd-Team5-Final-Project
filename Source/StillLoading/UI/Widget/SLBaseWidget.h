// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLSoundTypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLBaseWidget.generated.h"

class USLUISubsystem;
class USLTextPoolSubsystem;
class USLSoundSubsystem;
struct FSLWidgetActivateBuffer;

UCLASS()
class STILLLOADING_API USLBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem);
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer);
	virtual void DeactivateWidget() {};

	void ApplyOnChangedChapter(const FSLWidgetActivateBuffer& WidgetActivateBuffer);

	ESLInputModeType GetWidgetInputMode() const;
	int32 GetWidgetOrder() const;
	bool GetWidgetCursorMode() const;

protected:
	UFUNCTION()
	virtual void OnEndedOpenAnim() {};

	UFUNCTION()
	virtual void OnEndedCloseAnim();

	UFUNCTION()
	void NotifyChangedLanguage();

	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer);
	virtual void ApplyImageData();
	virtual void ApplyFontData() {};
	virtual void ApplyTextData() {};

	virtual bool ApplyBackgroundImage();
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle);
	virtual bool ApplySliderImage(FSliderStyle& SliderStyle);
	virtual bool ApplyBorderImage();
	virtual bool ApplyTextBorderImage();
	virtual bool ApplyProgressBarImage(FProgressBarStyle& ProgressBarStyle);
	virtual bool ApplyOtherImage();
	
	void PlayUISound(ESLUISoundType SoundType);
	void MoveToLevelByType(ESLLevelNameType LevelType);

	void CheckValidOfUISubsystem();
	void CheckValidOfTextPoolSubsystem();
	void CheckValidOfSoundSubsystem();

protected:
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<USLTextPoolSubsystem> TextPoolSubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<USLSoundSubsystem> SoundSubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> OpenAnim = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> CloseAnim = nullptr;

	UPROPERTY()
	TMap<ESLPublicWidgetImageType, UTexture2D*> PublicImageMap;

	UPROPERTY()
	FSlateFontInfo FontInfo;

	ESLChapterType CurrentChapter = ESLChapterType::EC_None;
	ESLInputModeType WidgetInputMode = ESLInputModeType::EIM_UIOnly;

	int32 WidgetOrder = 0;
	bool bIsVisibleCursor = true;

	FWidgetAnimationDynamicEvent EndOpenAnimDelegate;
	FWidgetAnimationDynamicEvent EndCloseAnimDelegate;
};
