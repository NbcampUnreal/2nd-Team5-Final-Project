// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLBaseTextPrintWidget.generated.h"

class UButton;
class UCanvasPanel;
class UTextBlock;

UCLASS()
class STILLLOADING_API USLBaseTextPrintWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;

protected:
	UFUNCTION()
	void OnClickedNextButton();

	void PrintTalkText();
	void ChangeTargetText();

public:
	UPROPERTY(EditAnywhere)
	float PrintTime = 0.3f;

protected:
	UPROPERTY()
	TObjectPtr<UButton> ParentNextButton = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> ParentNameText = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> ParentTalkText = nullptr;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> ParentNamePanel = nullptr;

	UPROPERTY()
	TArray<FName> NameArray;

	UPROPERTY()
	TArray<FText> TalkArray;

	FTimerHandle TextPrintTimer;
	FText TargetText = FText::GetEmpty();

	int32 TargetTextIndex = 0;
	int32 CurrentTextIndex = -1;
};
