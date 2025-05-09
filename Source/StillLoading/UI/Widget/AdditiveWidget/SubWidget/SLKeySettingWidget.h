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
	const TObjectPtr<UInputAction> InputAction = nullptr;
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

private:
	UFUNCTION()
	void ChangeKeyData(const FName& TargetName);

	void InitKeyDataMap();
	void AddToKeyDataMap(const FKey& NewKey, const UInputAction* NewInputAction);

	void InitKeyInfoWidgetMap();
	void GetIMCKey();
	void AddIMCKeyMap(const FString& ActionName, const FKey& Key);

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMC = nullptr;

protected:
	UPROPERTY()
	TMap<EInputActionType, FKey> IMCKeyMap;

	UPROPERTY()
	TMap<FName, USLKeyMappingWidget*> KeyInfoWidgetMap;

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
	TObjectPtr<USLKeyMappingWidget> Pause = nullptr;
};
