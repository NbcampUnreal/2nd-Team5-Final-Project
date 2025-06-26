#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RotateToFaceTarget.generated.h"

UCLASS()
class STILLLOADING_API UBTService_RotateToFaceTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RotateToFaceTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float InterpolationSpeed = 5.0f;
};
