// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLOptionWidget.generated.h"

class UButton;
class UTextBlock;
class USlider;
class UExpandableArea;
class UGameUserSettings;
class URendererSettings;

UCLASS()
class STILLLOADING_API USLOptionWidget : public USLAdditiveWidget
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

private:
	// Language Funtion
	UFUNCTION()
	void OnClickedKor();

	UFUNCTION()
	void OnClickedEng();
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

	// Key Setting Function
	UFUNCTION()
	void OnClickedKeySetting();
	//

	UFUNCTION()
	void OnClickedQuit();

	bool CheckValidOfGameUserSettings();
	bool CheckValidOfRendererSettings();

	void UpdateScreenModeButton();
	void UpdateLanguageButton();
	void UpdateResolution(int32 ResolutionNum);

private:
	// Language Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> KorButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> EngButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> KorText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> EngText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageModeText = nullptr;
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
	//

	// Brightness Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> BrightnessSlider = nullptr;
	//

	// Key Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> KeySettingButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> KeySettingText = nullptr;
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
	TObjectPtr<UGameUserSettings> GameUserSettings = nullptr;

	UPROPERTY()
	TObjectPtr<URendererSettings> RendererSettings = nullptr;

	bool bIsFullScreen = true;
	bool bIsKor = true;

	static const TArray<TPair<float, float>> ResolutionSet;
};
