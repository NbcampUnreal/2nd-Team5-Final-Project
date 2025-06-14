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
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType);

protected:
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 CurrentHp;
};
