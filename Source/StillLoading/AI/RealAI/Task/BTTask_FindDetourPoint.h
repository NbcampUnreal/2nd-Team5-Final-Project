#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindDetourPoint.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_FindDetourPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindDetourPoint();

protected:
	/** 원래 목표가 되는 액터의 블랙보드 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** 계산된 우회 지점을 저장할 블랙보드 키 (Vector 타입) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DetourLocationKey;

	/** 옆으로 우회할 거리 */
	UPROPERTY(EditAnywhere, Category = "AI")
	float DetourDistance = 400.0f;

	/** 우회 지점을 찾기 위한 탐색 반경 */
	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 300.0f;
    
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
