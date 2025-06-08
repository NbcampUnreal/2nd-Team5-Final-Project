#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateFormation.generated.h"

UCLASS()
class STILLLOADING_API UBTService_UpdateFormation : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_UpdateFormation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float FormationSpacing = 300.f;
};
