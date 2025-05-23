#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MontageDataAsset.generated.h"

UCLASS()
class STILLLOADING_API UMontageDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> SpecialAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AirAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> SkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> BlockMontage;
};
