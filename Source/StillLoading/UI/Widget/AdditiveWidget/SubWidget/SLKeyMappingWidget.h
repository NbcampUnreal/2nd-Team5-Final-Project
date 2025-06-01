// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character//DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLKeyMappingWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedKey, EInputActionType, KeyName);

UCLASS()
class STILLLOADING_API USLKeyMappingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitWidget(EInputActionType NewActionType, const FName& NewTagIndex, const FName& KeyText);
	void UpdateTextIndex(const FName& NewTagIndex);
	void UpdateTextFont(const FSlateFontInfo& FontInfo);
	void UpdateTagText(const FText& NewTagText);
	void UpdateKeyText(const FName& KeyText);

	void SetVisibilityButton(bool bIsVisible);
	void SetIsEnabledButton(bool bIsEnable);

	const FName& GetTagIndex() const;
	const EInputActionType GetActionType() const;

private:
	UFUNCTION()
	void OnClickedChangeKey();

	UFUNCTION()
	void PlayHoverSound();

public:
	UPROPERTY()
	FOnClickedKey KeyDelegate;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ChangeButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyTextBox = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TagText = nullptr;

	EInputActionType ActionType = EInputActionType::EIAT_None;
	FName TagIndex = "";
	FKey InputKey;
};
