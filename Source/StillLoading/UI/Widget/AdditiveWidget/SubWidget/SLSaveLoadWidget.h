// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "SLSaveLoadWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class STILLLOADING_API USLSaveLoadWidget : public USLOptionSubBase
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void OnUpdatedSettingValue() override;

protected:
	virtual void ApplyTextData() override;
	
};
