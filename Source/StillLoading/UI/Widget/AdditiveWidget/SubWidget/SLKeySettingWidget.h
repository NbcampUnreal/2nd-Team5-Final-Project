// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLKeySettingWidget.generated.h"

class USLKeyMappingWidget;
class UTextBlock;
class UButton;
class UImage;

USTRUCT(BlueprintType)
struct FSLKeySettingData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ActionName = "";

	UPROPERTY()
	EInputActionType ActionType = EInputActionType::None;

	UPROPERTY()
	FEnhancedActionKeyMapping MappingData;

	UPROPERTY()
	TObjectPtr<USLKeyMappingWidget> ElementWidget = nullptr;
};

UCLASS()
class STILLLOADING_API USLKeySettingWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent) override;

private:
	UFUNCTION()
	void OnClickedKeyDataButton(const FKey& TargetKey);

	void InitKeyDataMap();
	void AddToKeyDataMap(const FEnhancedActionKeyMapping& TargetData);
	void UpdateKeyMapping(const FKey& InputKey);

	void SetFocusToTargetButton(const FKey& TargetKey, bool bIsFocus);

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMC = nullptr;

private:
	UPROPERTY()
	TMap<FKey, FSLKeySettingData> KeyDataMap;


	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> MoveUp = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> MoveDown = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> MoveLeft = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> MoveRight = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Walk = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Jump = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Attack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Interaction = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Look = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> PointMove = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLKeyMappingWidget> Menu = nullptr;

	FKey TempKey;

	bool bOnClickedChange = false;
};
