// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "SLLanguageSettingWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class USLUserDataSubsystem;

UCLASS()
class STILLLOADING_API USLLanguageSettingWidget : public USLOptionSubBase
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplyListBackImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyLeftArrowImage(FButtonStyle& ButtonStyle) override;
	virtual bool ApplyRightArrowImage(FButtonStyle& ButtonStyle) override;

private:
	UFUNCTION()
	void OnClickedLanguageLeftButton();

	UFUNCTION()
	void OnClickedLanguageRightButton();

	void UpdateLanguage(bool bIsLeft);
	void UpdateLanguageText();

	void CheckValidOfUserDataSubsystem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ESLLanguageType, FText> LanguageTextMap;

private:
	// Language Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageModeText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> LanguageLeftButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> LanguageRightButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LanguageText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LanguageBack = nullptr;
	//

	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	static const FName LanguageTagIndex;
};
