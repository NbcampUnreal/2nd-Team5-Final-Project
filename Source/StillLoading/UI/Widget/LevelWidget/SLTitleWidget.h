// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUITypes.h"
#include "SLTitleWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UCanvasPanel;
class UNiagaraSystemWidget;
class URetainerBox;
class USLButtonWidget;
class USLSaveGameSubsystem;

UCLASS()
class STILLLOADING_API USLTitleWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnClickedStartButton();

	UFUNCTION()
	void OnClickedContinueButton();

	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void ApplyTextData() override;
	virtual bool ApplyBorderImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyOtherImage() override;

private:
	UFUNCTION()
	void OnClickedOptionButton();

	UFUNCTION()
	void OnClickedQuitButton();

	UFUNCTION()
	void OnClickedNewGameButton();

	UFUNCTION()
	void OnClickedCancleButton();


	void CheckValidOfSaveGameSubsystem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressValue = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressDuration = 0.1f;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> StartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> ContinueButton = nullptr;

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

	//
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> QuestionPanel = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> QuestionBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> QuestionText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> AgreeButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CancleButton = nullptr;
	//

	UPROPERTY()
	TMap<ESLTitlePrivateImageType, TObjectPtr<UObject>> PrivateImageMap;

	UPROPERTY()
	ESLLevelNameType NextLevelType = ESLLevelNameType::ELN_None;

	UPROPERTY()
	TObjectPtr<USLSaveGameSubsystem> SaveGameSubsystem = nullptr;

	static const FName TitleTextIndex;
	static const FName StartButtonIndex;
	static const FName ContinueButtonIndex;
	static const FName OptionButtonIndex;
	static const FName QuitButtonIndex;
	static const FName QuestionTextIndex;
};
