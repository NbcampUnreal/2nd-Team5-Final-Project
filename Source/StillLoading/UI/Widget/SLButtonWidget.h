// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubSystem/SLSoundTypes.h"
#include "SLButtonWidget.generated.h"

class UButton;
class UTextBlock;
class URetainerBox;
class UNiagaraSystem;
class UNiagaraSystemWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSLButtonClicked);

UCLASS()
class STILLLOADING_API USLButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitButton();
	void SetButtonStyle(FButtonStyle NewStyle);
	void SetButtonText(const FText& NewText);
	void SetButtonFont(FSlateFontInfo& NewFont, float FontOffset);
	void SetRetainerMat(UMaterialInterface* NewMaterial);
	void SetNiagaraWidget(UNiagaraSystem* NewNiagara);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnClickedButton();

	UFUNCTION()
	void OnHoveredButton();

	UFUNCTION()
	void OnUnhoveredButton();

	void PlayButtonSound(ESLUISoundType SoundType);

public:
	UPROPERTY(BlueprintAssignable, Category = "Button Property")
	FOnSLButtonClicked OnClicked;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> Button = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<URetainerBox> RetainerBox = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> NiagaraWidget = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> AddProgress = nullptr;

	UPROPERTY()
	TObjectPtr <UMaterialInstanceDynamic> ButtonDynamicMat = nullptr;

	bool bIsContainNiagara = false;
};
