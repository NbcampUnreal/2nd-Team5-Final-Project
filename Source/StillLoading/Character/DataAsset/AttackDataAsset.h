#pragma once

#include "CoreMinimal.h"
#include "Character/Interface/SLBattleInterface.h"
#include "Engine/DataAsset.h"
#include "AttackDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	EAttackAnimType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DamageAmount;
};

UCLASS()
class STILLLOADING_API UAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FAttackData> AttackDataList;
};
