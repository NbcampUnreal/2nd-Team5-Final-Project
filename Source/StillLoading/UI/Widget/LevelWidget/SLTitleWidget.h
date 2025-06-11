// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLTitleWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UNiagaraSystemWidget;
class URetainerBox;
class USLButtonWidget;

UCLASS()
class STILLLOADING_API USLTitleWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyBackgroundImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyOtherImage() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

	UFUNCTION()
	void OnClickedOptionButton();

	UFUNCTION()
	void OnClickedQuitButton();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressValue = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressDuration = 0.1f;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> StartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> OptionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> QuitButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TitleTextImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundBorder = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> BackgroundEffect = nullptr;

	static const FName TitleTextIndex;
	static const FName StartButtonIndex;
	static const FName OptionButtonIndex;
	static const FName QuitButtonIndex;
};
