// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLMoveToDirectionObj.generated.h"

enum class EHitAnimType : uint8;
class UBattleComponent;

UCLASS()
class STILLLOADING_API ASLMoveToDirectionObj : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLMoveToDirectionObj();

protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;
	
	UFUNCTION()
	virtual void OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);
	
	UPROPERTY(EditAnywhere, Category = "Movable Object")
	TObjectPtr<UBattleComponent> BattleComponent;
	
	UPROPERTY(EditAnywhere, Category = "Movable Object")
	float MoveForce = 1000.f;
	
};
