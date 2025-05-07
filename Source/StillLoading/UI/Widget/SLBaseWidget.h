// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/SLUITypes.h"
#include "SLBaseWidget.generated.h"

class USLUISubsystem;

UCLASS()
class STILLLOADING_API USLBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem);
	virtual void ApplyOnChangedLanguage() {};
	virtual void ApplyOnChangedChapter(int ChpaterNum) {};
	virtual void ActivateWidget() {};
	virtual void DeactivateWidget() {};

protected:
	UFUNCTION()
	virtual void OnEndedOpenAnim();

	UFUNCTION()
	virtual void OnEndedCloseAnim();

	void PlayOpenAnim();
	void PlayCloseAnim();
	void PlayUISound(ESLUISoundType SoundType);
	bool CheckValidOfUISubsystem();

protected:
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> OpenAnim = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> CloseAnim = nullptr;

	FWidgetAnimationDynamicEvent EndOpenAnimDelegate;
	FWidgetAnimationDynamicEvent EndCloseAnimDelegate;
};
