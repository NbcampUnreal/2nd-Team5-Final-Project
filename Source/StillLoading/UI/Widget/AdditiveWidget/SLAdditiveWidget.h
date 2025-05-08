// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SLBaseWidget.h"
#include "SLAdditiveWidget.generated.h"

UCLASS()
class STILLLOADING_API USLAdditiveWidget : public USLBaseWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	void CloseWidget();

	virtual void FindWidgetData() override;
};
