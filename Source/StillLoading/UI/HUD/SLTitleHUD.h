// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/SLBaseHUD.h"
#include "SLTitleHUD.generated.h"

UCLASS()
class STILLLOADING_API ASLTitleHUD : public ASLBaseHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MoveToStartMap();

	UFUNCTION()
	void NotifyTalkEnded();

	UFUNCTION()
	void NotifyChoiceEnded(bool bIsAccept);

protected:
	virtual void OnStartedHUD() override;
};
