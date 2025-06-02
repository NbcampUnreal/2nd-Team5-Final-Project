#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckStrategyTag.generated.h"

UCLASS()
class STILLLOADING_API UBTDecorator_CheckStrategyTag : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckStrategyTag();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "GameplayTag")
	FGameplayTag TagToCheck;
};
