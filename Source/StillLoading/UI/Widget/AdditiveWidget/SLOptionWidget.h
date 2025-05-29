// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLOptionWidget.generated.h"

class UButton;
class UTextBlock;
class USlider;
class UImage;
class UExpandableArea;
class UWidgetSwitcher;
class USLKeySettingWidget;
class UGameUserSettings;
class URendererSettings;
class USLUserDataSubsystem;

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
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle) override;
	virtual bool ApplySliderImage(FSliderStyle& SliderStyle) override;
	virtual bool ApplyBorderImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyOtherImage();

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
	
	// Language Function
	UFUNCTION()
	void OnClickedKor();

	UFUNCTION()
	void OnClickedEng();

	UFUNCTION()
	void OnClickedLanguageLeftButton();

	UFUNCTION()
	void OnClickedLanguageRightButton();
	//

	// Screen Mode Function
	UFUNCTION()
	void OnClickedFullScreen();

	UFUNCTION()
	void OnClickedWindowScreen();
	//

	// Resolution Function
	UFUNCTION()
	void OnClickedFirstResolution();

	UFUNCTION()
	void OnClickedSecondResolution();

	UFUNCTION()
	void OnClickedThirdResolution();
	//

	// Sound Volume Funtion
	UFUNCTION()
	void UpdateBgmVolume(float VolumeValue);

	UFUNCTION()
	void UpdateEffectVolume(float VolumeValue);
	//

	// Brightness Function
	UFUNCTION()
	void UpdateBrightness(float BrightnessValue);
	//

	UFUNCTION()
	void OnClickedQuit();

	void InitOptionVariable();
	void CheckValidOfUserDataSubsystem();
	void UpdateLanguage(bool bIsLeft);

	void UpdateScreenModeButton();
	void UpdateLanguageButton();
	void UpdateResolution(int32 ResolutionNum);

	void ApplyExpandableImage();
	void ApplyExpandedImage();
	void ApplyListBackImage();
	void ApplyLeftArrowImage();
	void ApplyRightArrowImage();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ESLLanguageType, FText> LanguageTextMap;

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
	TObjectPtr<UButton> LanguageSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> GraphicSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SoundSetBt = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> KeySettingButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageSetText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> GraphicSetText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> SoundSetText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> KeySettingText = nullptr;
	//

	// Language Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageModeText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> LanguageLeftButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> LanguageRightButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LanguageBack = nullptr;
	//

	// Screen Mode Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> FullScreenButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> WindowScreenButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> WindowModeText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> FullScreenText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> WindowScreenText = nullptr;
	//

	// Resolution Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UExpandableArea> ResolutionList = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> FirstResolutionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SecondResolutionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ThirdResolutionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ResolutionText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentResolutionText = nullptr;
	//

	// Sound Volume Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> BgmVolumeSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> EffectVolumeSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> BgmVolumeText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectVolumeText = nullptr;
	//

	// Brightness Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> BrightnessSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> BrightnessText = nullptr;
	//

	// Key Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeySettingWidget> KeySettingWidget = nullptr;
	//

	// Widget Handle
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> QuitGameText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> CloseText = nullptr;
	//

	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	static const TArray<TPair<float, float>> ResolutionSet;

	static const FName TitleTextIndex;
	static const FName LanguageTagIndex;
	static const FName WindowModeTagIndex;
	static const FName ResolutionTagIndex;
	static const FName BgmTagIndex;
	static const FName EffectTagIndex;
	static const FName BrigthnessTagIndex;
	static const FName KorButtonIndex;
	static const FName EngButtonIndex;
	static const FName FullScreenButtonIndex;
	static const FName WindowedButtonIndex;
	static const FName KeySettingButtonIndex;
	static const FName QuitGameButtonIndex;
	static const FName CloseButtonIndex;
};
