// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "SLTalkWidget.generated.h"

class UImage;

UCLASS()
class STILLLOADING_API USLTalkWidget : public USLBaseTextPrintWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyTextBorderImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle);

private:
	void UpdateTalkState(ESLTalkTargetType TalkTargetType, const FName& TargetName, const FName& TalkName);

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SkipButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> FastButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TalkText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TalkBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> NameBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> NamePanel = nullptr;


	ESLTalkTargetType CurrentTalkType = ESLTalkTargetType::ETT_None;
	FName CurrentTalkTarget = "";
	FName CurrentTalkName = "";
};
