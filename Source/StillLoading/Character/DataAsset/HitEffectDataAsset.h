#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HitEffectDataAsset.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class STILLLOADING_API UHitEffectDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DefaultEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> EmpoweredEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> KillMotionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> CharacterHitEffect;
};
