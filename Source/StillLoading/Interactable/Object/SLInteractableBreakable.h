// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"

#include "SLInteractableBreakable.generated.h"

class UNiagaraSystem;

UCLASS()
class STILLLOADING_API ASLInteractableBreakable : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType);

private:
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	int8 MaxHp;
	int8 CurrentHp;
};
