#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttackComboDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAttackComboData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SectionName;
};

UCLASS()
class STILLLOADING_API UAttackComboDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttackComboData> ComboChain;
};
