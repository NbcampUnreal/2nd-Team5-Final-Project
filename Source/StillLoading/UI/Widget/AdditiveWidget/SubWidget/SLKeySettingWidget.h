// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLKeySettingWidget.generated.h"


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
	void ChangeKeyData();
	void GetIMCKey();

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMC = nullptr;

protected:
	UPROPERTY()
	TMap<EInputActionType, FKey> IMCKeyMap;
};
