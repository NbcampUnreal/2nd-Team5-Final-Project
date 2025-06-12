// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SLBaseTextPrintWidget.generated.h"

class UButton;
class UCanvasPanel;
class UTextBlock;

UCLASS()
class STILLLOADING_API USLBaseTextPrintWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

	FSLTalkDelegateBuffer& GetTalkDelegateBuffer();

protected:
	UFUNCTION()
	void OnClickedNextButton();

	UFUNCTION()
	void OnClickedSkipButton();

	UFUNCTION()
	void OnClickedFastButton();

	void PrintTalkText();
	void ChangeTargetText();

public:
	UPROPERTY(EditAnywhere)
	float PrintTime = 0.1f;

	UPROPERTY(EditAnywhere)
	float Accelerator = 2.0f;

protected:
	UPROPERTY(BlueprintReadOnly)
	FSLTalkDelegateBuffer TalkDelegateBuffer;

	UPROPERTY()
	TObjectPtr<UButton> ParentNextButton = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> ParentNameText = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> ParentTalkText = nullptr;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> ParentNamePanel = nullptr;

	UPROPERTY()
	TObjectPtr<UButton> ParentSkipButton = nullptr;

	UPROPERTY()
	TObjectPtr<UButton> ParentFastButton = nullptr;

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
