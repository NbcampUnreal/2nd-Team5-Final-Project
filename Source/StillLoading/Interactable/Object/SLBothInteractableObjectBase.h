// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "Interactable/Object/SLInteractableBreakable.h"
#include "SLBothInteractableObjectBase.generated.h"

enum class EHitAnimType : uint8;
class UBattleComponent;
class UNiagaraSystem;
class USoundBase;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteraction);

UCLASS()
class STILLLOADING_API ASLBothInteractableObjectBase : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLBothInteractableObjectBase();
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Events")
	void OnDestroied();

	UFUNCTION()
	virtual void OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	void InComingAttack();

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectBreaked OnObjectBreaked;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectHit OnObjectHit;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteraction OnInteraction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject")
	TObjectPtr<UBoxComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (ClampMin = "0"))
	int32 CurrentHp;

	UPROPERTY(EditAnywhere, Category = "Effects")
	float EffectScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsNotHidden = false;

};
