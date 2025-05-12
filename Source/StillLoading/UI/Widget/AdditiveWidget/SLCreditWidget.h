// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLCreditWidget.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;
class USLCreditTextWidget;

UCLASS()
class STILLLOADING_API USLCreditWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USLCreditTextWidget> CreditTextWidgetClass = nullptr;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UScrollBox> CreditBox = nullptr;
};
