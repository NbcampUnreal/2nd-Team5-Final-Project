#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToAvailableCircleSlot.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_MoveToAvailableCircleSlot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToAvailableCircleSlot();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Formation")
	float Radius = 400.f;

	UPROPERTY(EditAnywhere, Category = "Formation")
	int32 NumSlots = 6;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, Category = "Collision")
	float ProbeRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Tick")
	float UpdateInterval = 0.5f;
};