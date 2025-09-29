// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLMainTitleWidget.generated.h"

class UImage;

UCLASS()
class STILLLOADING_API USLMainTitleWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void ApplyTextData() override;
	virtual bool ApplyOtherImage() override;

private:
	UFUNCTION()
	void OnClickedStartButton();

	UFUNCTION()
	void OnClickedLoadButton();

	UFUNCTION()
	void OnClickedOptionButton();

	UFUNCTION()
	void OnClickedQuitButton();

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> StartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> LoadButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> OptionButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> QuitButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LogoImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY()
	TMap<ESLTitlePrivateImageType, TObjectPtr<UObject>> PrivateImageMap;

	static const FName StartButtonIndex;
	static const FName LoadButtonIndex;
	static const FName OptionButtonIndex;
	static const FName QuitButtonIndex;
};
