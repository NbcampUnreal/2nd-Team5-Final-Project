#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "SLBTTask_MoveToWithRotation.generated.h"

UCLASS()
class STILLLOADING_API USLBTTask_MoveToWithRotation : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	USLBTTask_MoveToWithRotation();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Move")
	float MaxMoveDuration = 1.5f;

private:
	float TimeElapsed = 0.f;
};
