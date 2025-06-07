// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetSpiralLocation.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UBTTask_GetSpiralLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetSpiralLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** 나선의 회전 수 */
	UPROPERTY(EditAnywhere, Category = "Spiral")
	float NumberOfTurns;

	/** 총 생성할 점의 개수 */
	UPROPERTY(EditAnywhere, Category = "Spiral")
	int32 TotalPoints;

	/** 최소 반지름 */
	UPROPERTY(EditAnywhere, Category = "Spiral")
	float MinRadius;

	/** 최대 반지름 */
	UPROPERTY(EditAnywhere, Category = "Spiral")
	float MaxRadius;

	/** 현재 인덱스를 저장/읽을 블랙보드 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CurrentIndexKey;

	/** 계산된 위치를 저장할 블랙보드 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	/** 모든 점을 방문했는지 여부를 저장할 블랙보드 키 (선택사항) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsCompleteKey;

	/** 나선 중심점 (기본값: AI 위치) */
	UPROPERTY(EditAnywhere, Category = "Spiral")
	bool bUseActorLocation;

	/** 고정 중심점 (bUseActorLocation이 false일 때) */
	UPROPERTY(EditAnywhere, Category = "Spiral", meta = (EditCondition = "!bUseActorLocation"))
	FVector FixedCenterLocation;

private:
	/** 주어진 인덱스에 해당하는 나선형 위치 계산 */
	FVector CalculateSpiralPoint(int32 Index, const FVector& Center) const;
};