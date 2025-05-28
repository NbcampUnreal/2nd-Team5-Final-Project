#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckGamePlayTag.generated.h"

UCLASS()
class STILLLOADING_API UBTDecorator_CheckGamePlayTag : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckGamePlayTag();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "GameplayTag")
	FGameplayTag TagToCheck;
};
