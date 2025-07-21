// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLCreditWidget.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;
class UImage;
class USLCreditTextWidget;
class USLButtonWidget;

UCLASS()
class STILLLOADING_API USLCreditWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;

	virtual bool ApplyBackgroundImage(FSlateBrush& SlateBrush) override;

private:
	UFUNCTION()
	void OnClickedCloseButton();

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USLCreditTextWidget> CreditTextWidgetClass = nullptr;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UScrollBox> CreditBox = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;
};
