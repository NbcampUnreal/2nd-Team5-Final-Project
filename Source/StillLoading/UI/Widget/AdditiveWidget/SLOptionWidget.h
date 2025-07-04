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
class USLLevelTransferSubsystem;

UCLASS()
class STILLLOADING_API USLOptionWidget : public USLAdditiveWidget
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void ApplyTextData() override;
	virtual bool ApplyOtherImage() override;

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
	void OnClickedMoveToTitle();

	UFUNCTION()
	void OnClickedRestart();

	UFUNCTION()
	void OnClickedSettingReset();

	void CheckValidOfLevelTransferSubsystem();

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
	TObjectPtr<USLButtonWidget> RestartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> MoveToTitleButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> SettingResetButton = nullptr;
	//

	UPROPERTY()
	TMap<ESLOptionPrivateImageType, TObjectPtr<UObject>> PrivateImageMap;

	UPROPERTY()
	TObjectPtr<USLLevelTransferSubsystem> LevelTransferSubsystem = nullptr;

	static const FName TitleTextIndex;
	static const FName KeySettingButtonIndex;
	static const FName RestartButtonIndex;
	static const FName MoveToTitleButtonIndex;
	static const FName CloseButtonIndex;
	static const FName SettingResetButtonIndex;
	static const FName LanguageSettingIndex;
	static const FName GraphicSettingIndex;
	static const FName SoundSettingIndex;
};
