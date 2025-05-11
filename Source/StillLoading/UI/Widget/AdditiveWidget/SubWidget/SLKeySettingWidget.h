// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "Character//DynamicIMCComponent/SLDynamicIMCComponent.h"
//#include "EnhancedActionKeyMapping.h"
#include "SLKeySettingWidget.generated.h"

class USLUserDataSubsystem;
class USLKeyMappingWidget;
class UTextBlock;
class UButton;
class UImage;

UCLASS()
class STILLLOADING_API USLKeySettingWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void ActivateWidget(ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent) override;

private:
	UFUNCTION()
	void OnClickedKeyDataButton(EInputActionType TargetAction);

	void InitElementWidget();
	void UpdateKeyMapping(const FKey& InputKey);
	void SetFocusToTargetButton(EInputActionType TargetAction, bool bIsFocus);

	void CheckValidOfUserDateSubsystem();

private:
	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	UPROPERTY()
	TMap<EInputActionType, USLKeyMappingWidget*> ActionWidgetMap;

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

	static const FName MoveUpTagIndex;
	static const FName MoveDownTagIndex;
	static const FName MoveLeftTagIndex;
	static const FName MoveRightTagIndex;
	static const FName WalkTagIndex;
	static const FName JumpTagIndex;
	static const FName AttackTagIndex;
	static const FName InteractionTagIndex;
	static const FName PointMoveTagIndex;
	static const FName LookTagIndex;
	static const FName MenuTagIndex;

	EInputActionType TargetActionType = EInputActionType::EIAT_None;
	bool bOnClickedChange = false;
};
