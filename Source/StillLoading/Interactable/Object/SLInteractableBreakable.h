// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"

#include "SLInteractableBreakable.generated.h"

enum class EHitAnimType : uint8;
class UBattleComponent;
class UNiagaraSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectBreaked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectHit, const FHitResult&, HitResult);

UCLASS()
class STILLLOADING_API ASLInteractableBreakable : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLInteractableBreakable();
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectBreaked OnObjectBreaked;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectHit OnObjectHit;
	
protected:
	UFUNCTION()
	virtual void OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;
	UPROPERTY(EditAnywhere, Category = "Effects")
	float EffectScale = 1.0f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleComponent> BattleComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsNotHidden = false;
};
