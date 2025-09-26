// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLSaveSlotWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedSlot, int32, SlotNumber);

UCLASS()
class STILLLOADING_API USLSaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitWidget(int32 Number);
	void UpdateTextFont(const FSlateFontInfo& FontInfo, float FontOffset);

	void SetEmptyMode(bool bIsEmpty);
	void SetSlotImage(UObject* ImageObj);
	void SetMapImage(UObject* ImageObj);
	void SetChapterNumber(const FText& Chapter);
	void SetMapName(const FText& Name);
	void SetPlayTime(const float TimeValue);
	void SetSaveTime(const FDateTime& TimeValue);

private:
	UFUNCTION()
	void OnClickedSlot();

public:
	UPROPERTY(BlueprintAssignable)
	FOnClickedSlot OnClicked;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> EmptyCover = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SlotButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> SlotImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> MapImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotNumber = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ChapterNumber = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> MapName = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayTime = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> SaveTime = nullptr;

	int32 SlotNum = 0;
};
