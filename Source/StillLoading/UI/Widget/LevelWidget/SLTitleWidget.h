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
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

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

	static const FName BackgroundImgIndex;
	static const FName TitleTextImgIndex;
	static const FName ButtonsBackgroundImgIndex;
	static const FName ButtonImgIndex;
};
