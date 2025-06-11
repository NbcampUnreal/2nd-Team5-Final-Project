// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLOptionWidget.generated.h"

class UTextBlock;
class UImage;
class UWidgetSwitcher;
class USLButtonWidget;
class USLLanguageSettingWidget;
class USLGraphicSettingWidget;
class USLSoundSettingWidget;
class USLKeySettingWidget;

UCLASS()
class STILLLOADING_API USLOptionWidget : public USLAdditiveWidget
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyBorderImage(FSlateBrush& SlateBrush) override;

	virtual void OnEndedCloseAnim() override;

private:
	// Layer Switch Function
	UFUNCTION()
	void OnClickedLanguageSetting();

	UFUNCTION()
	void OnClickedGraphicSetting();

	UFUNCTION()
	void OnClickedSoundSetting();

	UFUNCTION()
	void OnClickedKeySetting();
	//

	UFUNCTION()
	void OnClickedQuit();


private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> OptionPanelBack = nullptr;

	// Layer Switch Handle
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> LayerSwitcher = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> LanguageSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> GraphicSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> SoundSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> KeySettingButton = nullptr;
	//

	// Layer Widget
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLLanguageSettingWidget> LanguageSettingWidget = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLGraphicSettingWidget> GraphicSettingWidget = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLSoundSettingWidget> SoundSettingWidget = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeySettingWidget> KeySettingWidget = nullptr;
	//

	// Widget Handle
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> QuitGameButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CloseButton = nullptr;
	//

	static const FName TitleTextIndex;
	static const FName KeySettingButtonIndex;
	static const FName QuitGameButtonIndex;
	static const FName CloseButtonIndex;
	static const FName LanguageSettingIndex;
	static const FName GraphicSettingIndex;
	static const FName SoundSettingIndex;
};
