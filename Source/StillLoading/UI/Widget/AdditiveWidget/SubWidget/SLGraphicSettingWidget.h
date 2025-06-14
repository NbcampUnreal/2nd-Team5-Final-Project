// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "SLGraphicSettingWidget.generated.h"

class UButton;
class UTextBlock;
class USlider;
class UExpandableArea;
class USLUserDataSubsystem;

UCLASS()
class STILLLOADING_API USLGraphicSettingWidget : public USLOptionSubBase
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle) override;
	virtual bool ApplySliderImage(FSliderStyle& SliderStyle) override;
	virtual bool ApplyExpandableImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyExpandedImage(FButtonStyle& ButtonStyle) override;

private:
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
	
	// Brightness Function
	UFUNCTION()
	void UpdateBrightness(float BrightnessValue);
	//

	void UpdateScreenModeButton();
	void UpdateResolution(int32 ResolutionNum);

	

	void CheckValidOfUserDataSubsystem();

private:
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

	// Brightness Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> BrightnessSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> BrightnessText = nullptr;
	//

	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	static const TArray<TPair<float, float>> ResolutionSet;

	static const FName WindowModeTagIndex;
	static const FName ResolutionTagIndex;
	static const FName BrigthnessTagIndex;
	static const FName FullScreenButtonIndex;
	static const FName WindowedButtonIndex;
};
