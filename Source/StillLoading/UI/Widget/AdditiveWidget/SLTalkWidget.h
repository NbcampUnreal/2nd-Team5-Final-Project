// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/SLUITypes.h"
#include "SLTalkWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovedNextTalk, int32, TalkIndex);

UCLASS()
class STILLLOADING_API USLTalkWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

	void UpdateTalkState(ESLTalkTargetType TalkTargetType, int32 TargetIndex);

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;

private:
	UFUNCTION()
	void OnClickedNextButton();

	void PrintTalkText();
	void ChangeTargetText();

public:
	UPROPERTY(EditAnywhere)
	float PrintTime = 0.3f;

	UPROPERTY()
	FOnMovedNextTalk TalkDelegate;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

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


	UPROPERTY()
	TArray<FName> NameArray;

	UPROPERTY()
	TArray<FText> TalkArray;

	FTimerHandle TextPrintTimer;
	FText TargetText = FText::GetEmpty();
	ESLTalkTargetType CurrentTalkType = ESLTalkTargetType::ETT_None;
	int32 CurrentTalkIndex = 0;
	int32 TargetTextIndex = 0;
	int32 CurrentTextIndex = -1;
};
