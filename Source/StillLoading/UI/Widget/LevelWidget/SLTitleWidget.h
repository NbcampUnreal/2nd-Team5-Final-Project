// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLTitleWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class STILLLOADING_API USLTitleWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyTextData() override;

	virtual bool ApplyBackgroundImage() override;
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle) override;
	virtual bool ApplyBorderImage() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

	UFUNCTION()
	void OnClickedOptionButton();

	UFUNCTION()
	void OnClickedQuitButton();

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> StartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> OptionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> QuitButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> StartText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> QuitText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TitleTextImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ButtonsBackground = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundBorder = nullptr;

	static const FName TitleTextIndex;
	static const FName StartButtonIndex;
	static const FName OptionButtonIndex;
	static const FName QuitButtonIndex;
};
