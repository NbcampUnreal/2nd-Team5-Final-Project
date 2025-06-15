// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SLBaseTextPrintWidget.generated.h"

class UButton;
class USLButtonWidget;
class UCanvasPanel;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTalkEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceEnded, bool, bIsAccept);

UCLASS()
class STILLLOADING_API USLBaseTextPrintWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

protected:
	UFUNCTION()
	void OnClickedNextButton();

	UFUNCTION()
	void OnClickedSkipButton();

	UFUNCTION()
	void OnClickedFastButton();

	UFUNCTION()
	void OnClickedAcceptButton();

	UFUNCTION()
	void OnClickedRejectButton();

	void PrintTalkText();
	void ChangeTargetText();
	void SetChoiceVisibility(bool bIsVisible);
	void CloseTalk();

public:
	UPROPERTY(BlueprintAssignable)
	FOnTalkEnded OnTalkEnded;

	UPROPERTY(BlueprintAssignable)
	FOnChoiceEnded OnChoiceEnded;

	UPROPERTY(EditAnywhere)
	float PrintTime = 0.1f;

	UPROPERTY(EditAnywhere)
	float Accelerator = 2.0f;

protected:
	UPROPERTY()
	TObjectPtr<UTextBlock> ParentNameText = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> ParentTalkText = nullptr;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> ParentNamePanel = nullptr;

	UPROPERTY()
	TObjectPtr<UButton> ParentNextButton = nullptr;

	UPROPERTY()
	TObjectPtr<USLButtonWidget> ParentSkipButton = nullptr;

	UPROPERTY()
	TObjectPtr<USLButtonWidget> ParentFastButton = nullptr;

	UPROPERTY()
	TObjectPtr<USLButtonWidget> ParentAcceptButton = nullptr;

	UPROPERTY()
	TObjectPtr<USLButtonWidget> ParentRejectButton = nullptr;

	UPROPERTY()
	TArray<FName> NameArray;

	UPROPERTY()
	TArray<FText> TalkArray;

	FTimerHandle TextPrintTimer;
	FText TargetText = FText::GetEmpty();

	int32 TargetTextIndex = 0;
	int32 CurrentTextIndex = -1;

	bool bIsFasted = false;
};
