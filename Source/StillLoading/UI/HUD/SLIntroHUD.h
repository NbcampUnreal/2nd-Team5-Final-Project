// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/SLBaseHUD.h"
#include "SLIntroHUD.generated.h"

UCLASS()
class STILLLOADING_API ASLIntroHUD : public ASLBaseHUD
{
	GENERATED_BODY()
	
protected:
	virtual void OnStartedHUD() override;
	virtual void InitLevelWidget() override;
};
