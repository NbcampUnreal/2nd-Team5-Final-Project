// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectDoppelganger.generated.h"


class UWidgetComponent;

UCLASS()
class STILLLOADING_API ASLReactiveObjectDoppelganger : public ASLBaseReactiveObject
{
	GENERATED_BODY()
public:
	ASLReactiveObjectDoppelganger();

protected:
	virtual void OnReacted(const ASLBaseCharacter* InCharacter);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> ProgressWidget;

};
