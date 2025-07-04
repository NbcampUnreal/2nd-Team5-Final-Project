// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLMapElementWidget.generated.h"

class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedMapElement, ESLLevelNameType, MapElementName);

UCLASS()
class STILLLOADING_API USLMapElementWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitMapElement(ESLLevelNameType NewType);
	void SetMapElementImage(UTexture2D* ImageSource);
	void SetIsEnabledButton(bool bIsEndabled);

private:
	UFUNCTION()
	void OnClickedMapElementButton();

public:
	UPROPERTY()
	FOnClickedMapElement OnClickedMapElement;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ElementButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ElementImg = nullptr;

	ESLLevelNameType ElementType = ESLLevelNameType::ELN_None;
};
