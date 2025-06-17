// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLStoryWidget.generated.h"

class UImage;
class UTextBlock;
class UWidgetSwitcher;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoryEnded);

UCLASS()
class STILLLOADING_API USLStoryWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetNextStoryText();

	UFUNCTION(BlueprintCallable)
	void SetStoryVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	int32 GetTextSize() const;

	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyTextBorderImage(FSlateBrush& SlateBrush) override;
	//virtual bool ApplyButtonImage(FButtonStyle& ButtonStyle);

private:
	UFUNCTION()
	void ChangeTargetText();

	UFUNCTION()
	void ChangeDissolveText();

	UFUNCTION()
	void OnChangedText();

	void UpdateStoryState(ESLStoryType TargetStoryType, const FName& TargetIndex);

public:
	UPROPERTY(BlueprintAssignable)
	FOnStoryEnded OnStoryEnded;

private:
	/*UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> NextButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> SkipButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> FastButton = nullptr;*/

	/*UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText = nullptr;*/

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> BottomStoryText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BottomStoryBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> MidStoryText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> MidStoryBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> DissolveTextAnim  = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ChangeTextAnim = nullptr;

	UPROPERTY()
	TObjectPtr<UTextBlock> TargetText = nullptr;

	UPROPERTY()
	TArray<FName> NameArray;

	UPROPERTY()
	TArray<FText> StoryArray;

	FWidgetAnimationDynamicEvent EndDissolveAnimDelegate;
	FWidgetAnimationDynamicEvent EndChangeAnimDelegate;

	/*UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> NameBack = nullptr;*/

	/*UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> NamePanel = nullptr;*/

	/*UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> StoryImg = nullptr;*/

	FText CurrentText = FText::GetEmpty();
	int32 CurrentTextIndex = 0;

	ESLStoryType CurrentStoryType = ESLStoryType::ES_Start;
	FName CurrentStoryName = "";

	bool bIsMidFirst = true;
};
