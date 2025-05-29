#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_AttackCooldown.generated.h"

UCLASS()
class STILLLOADING_API UBTDecorator_AttackCooldown : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTDecorator_AttackCooldown();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float Cooldown = 2.0f;
};
