// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectBreakable.generated.h"

class UNiagaraSystem;

UCLASS()
class STILLLOADING_API ASLReactiveObjectBreakable : public ASLBaseReactiveObject
{
	GENERATED_BODY()
	
public:
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType);

protected:
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 CurrentHp;
};
