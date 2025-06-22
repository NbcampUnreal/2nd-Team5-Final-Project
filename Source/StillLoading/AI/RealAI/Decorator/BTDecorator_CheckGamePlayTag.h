#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckGamePlayTag.generated.h"

UENUM(BlueprintType)
enum class ETagType : uint8
{
	TT_State 	UMETA(DisplayName = "State"),
	TT_Battle	UMETA(DisplayName = "Battle"),
	TT_Strategy	UMETA(DisplayName = "Strategy"),
	TT_MonsterMode	UMETA(DisplayName = "MonsterMode"),
};

UCLASS()
class STILLLOADING_API UBTDecorator_CheckGamePlayTag : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckGamePlayTag();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Tag Type")
	ETagType TagType;
	
	UPROPERTY(EditAnywhere, Category = "GameplayTag")
	FGameplayTag TagToCheck;
};
