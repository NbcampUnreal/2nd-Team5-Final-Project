#pragma once

#include "CoreMinimal.h"
#include "Interactable/Object/SLInteractableBreakable.h"
#include "SLDestructibleActor.generated.h"

class UNiagaraSystem;
class USoundBase;
class UDecalComponent;

UCLASS()
class STILLLOADING_API ASLDestructibleActor : public ASLInteractableBreakable
{
	GENERATED_BODY()

public:
	ASLDestructibleActor();

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	void TakeDamage();

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	bool CanTakeDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	void AddCrackDecal(const FVector& HitLocation, const FVector& HitNormal);

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	void ClearAllCracks();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

	void PlayHitFeedback(const FVector& HitLocation);
	void FlashDamagedMaterial();
	void StartShakeEffect();
	void UpdateShakeEffect(float DeltaTime);
	void DestroyActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterialInterface> NormalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterialInterface> DamagedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float MaterialFlashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crack Effects")
	TObjectPtr<UMaterialInterface> CrackDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crack Effects")
	FVector CrackDecalSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crack Effects")
	float CrackDecalLifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeFrequency;

private:
	TArray<TObjectPtr<UDecalComponent>> CrackDecals;
	FTimerHandle MaterialFlashTimerHandle;
	FTimerHandle ShakeTimerHandle;
	FVector OriginalLocation;
	float ShakeTimeElapsed;
	bool bIsShaking;
};