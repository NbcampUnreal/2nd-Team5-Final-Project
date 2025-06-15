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
class USLButtonWidget;
struct FSLWidgetActivateBuffer;

UCLASS()
class STILLLOADING_API USLBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem);
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer);
	virtual void DeactivateWidget();

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

	UFUNCTION()
	void PlayHoverSound();

	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer);
	virtual void ApplyImageData();
	virtual void ApplyFontData();
	virtual void ApplyTextData() {};

	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle);
	virtual bool ApplyButtonNormal(FButtonStyle& ButtonStyle);
	virtual void ApplyButtonHover(FButtonStyle& ButtonStyle, bool bIsActiveNormal);
	virtual void ApplyButtonRetainer(USLButtonWidget* ButtonWidget);
	virtual void ApplyButtonNiagara(USLButtonWidget* ButtonWidget);

	virtual bool ApplyBackgroundImage(FSlateBrush& SlateBrush);
	virtual bool ApplyBorderImage(FSlateBrush& SlateBrush);
	virtual bool ApplyTextBorderImage(FSlateBrush& SlateBrush);
	virtual bool ApplyOtherImage();
	
	void PlayUISound(ESLUISoundType SoundType);
	void MoveToLevelByType(ESLLevelNameType LevelType, bool bIsFade = true);

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
	TMap<ESLPublicWidgetImageType, UObject*> PublicAssetMap;

	UPROPERTY()
	FSlateFontInfo FontInfo;

	UPROPERTY()
	float FontOffset = 0.0f;

	ESLChapterType CurrentChapter = ESLChapterType::EC_None;
	ESLInputModeType WidgetInputMode = ESLInputModeType::EIM_UIOnly;

	int32 WidgetOrder = 0;
	bool bIsVisibleCursor = true;

	FWidgetAnimationDynamicEvent EndOpenAnimDelegate;
	FWidgetAnimationDynamicEvent EndCloseAnimDelegate;
};
