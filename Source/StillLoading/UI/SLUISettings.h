// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SLUISettings.generated.h"


UCLASS(Config = UISetting, DefaultConfig, meta = (DisplayName = "UISubsystemSettings"))
class STILLLOADING_API USLUISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetClass")
	int TestValue = 0;
};
