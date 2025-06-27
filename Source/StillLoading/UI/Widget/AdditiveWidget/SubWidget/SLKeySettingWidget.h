// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "Character//DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLKeySettingWidget.generated.h"

class USLUserDataSubsystem;
class USLKeyMappingWidget;
class UTextBlock;
class UGridPanel;
class UButton;
class UImage;

UCLASS()
class STILLLOADING_API USLKeySettingWidget : public USLOptionSubBase
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void OnUpdatedSettingValue() override;

protected:
	virtual void ApplyFontData() override;
	virtual void ApplyTextData() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent) override;

private:
	UFUNCTION()
	void OnClickedKeyDataButton(EInputActionType TargetAction);

	void InitElementWidget();
	void UpdateKeyMapping(const FKey& InputKey);
	void SetFocusToTargetButton(EInputActionType TargetAction, bool bIsFocus);

	void CheckValidOfUserDateSubsystem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TSubclassOf<USLKeyMappingWidget> KeyMappingWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TMap<EInputActionType, FName> KeyTagIndexMap;

private:
	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	UPROPERTY()
	TArray<USLKeyMappingWidget*> MappingWidgets;

	UPROPERTY()
	TArray<USLKeyMappingWidget*> MappingWidgetArray;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UGridPanel> KeySettingGrid = nullptr;

	EInputActionType TargetActionType = EInputActionType::EIAT_None;
	bool bOnClickedChange = false;
};
