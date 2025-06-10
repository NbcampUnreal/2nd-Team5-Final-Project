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
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle) override;
	virtual bool ApplyOtherImage() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

	UFUNCTION()
	void OnClickedOptionButton();

	UFUNCTION()
	void OnClickedQuitButton();

	UFUNCTION()
	void OnHoveredStartButton();

	UFUNCTION()
	void OnHoveredOptionButton();

	UFUNCTION()
	void OnHoveredQuitButton();

	UFUNCTION()
	void OnUnhorveredButton();

	void MappingButtonMaterial();
	void CheckButtonMaterial(UMaterialInstanceDynamic* ButtonMaterial);
	void PlayHoverEvent();
	void SetHoverTimer();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressValue = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Option")
	float ProgressDuration = 0.1f;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> StartButtonWidget = nullptr;

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
	TObjectPtr<UImage> TitleTextImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundBorder = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> StartButtonEffect = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> OptionButtonEffect = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> QuitButtonEffect = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<URetainerBox> StartRetainer = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<URetainerBox> OptionRetainer = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<URetainerBox> QuitRetainer = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> BackgroundEffect = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMat = nullptr;

	UPROPERTY()
	TMap<UButton*, UMaterialInstanceDynamic*> ButtonDMIMap;

	static const FName TitleTextIndex;
	static const FName StartButtonIndex;
	static const FName OptionButtonIndex;
	static const FName QuitButtonIndex;

	FTimerHandle HoverTimer;
	FName ProgressName = "Progress";
	float CurrentProgress = 0.0f;
	bool bIsContainEffect = false;
};
