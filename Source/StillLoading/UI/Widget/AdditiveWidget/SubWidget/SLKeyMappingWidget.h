// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLKeyMappingWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedKey, const FKey&, KeyName);

UCLASS()
class STILLLOADING_API USLKeyMappingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitWidget(const FName& NewTagText, const FKey& NewKey);
	void UpdateTagText(const FName& NewTagText);
	void UpdateKeyText(const FKey& NewKey);

private:
	UFUNCTION()
	void OnClickedChangeKey();

public:
	FOnClickedKey KeyDelegate;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ChangeButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UEditableTextBox> KeyTextBox = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TagText = nullptr;

	FKey InputKey;
	EInputActionType ActionType = EInputActionType::None;
};
