// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "SLTalkWidget.generated.h"

class UImage;
class UWidgetSwitcher;

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

private:
	void UpdateTalkState(ESLTalkTargetType TalkTargetType, const FName& TargetName, const FName& TalkName);
	void ChangeTalkLayer(ESLChapterType ChapterType);

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> SkipButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> FastButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> AcceptButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> RejectButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher = nullptr;

	// Left Layer
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftNameText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftTalkText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LeftTalkBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LeftNameBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> LeftNamePanel = nullptr;
	//

	// Mid Layer
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> MidNameText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> MidTalkText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> MidTalkBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> MidNameBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MidNamePanel = nullptr;
	//

	ESLTalkTargetType CurrentTalkType = ESLTalkTargetType::ETT_None;
	FName CurrentTalkTarget = "";
	FName CurrentTalkName = "";
};
