// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayRandomMontage.generated.h"

/**
 * 몽타주가 완료될 때까지 기다리는 랜덤 몽타주 실행 태스크
 */
UCLASS()
class STILLLOADING_API UBTTask_PlayRandomMontage : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PlayRandomMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
    
	// 몽타주 완료 감지용 핸들러
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 재생할 몽타주 배열
	UPROPERTY(EditAnywhere, Category = "Animation")
	TArray<TObjectPtr<UAnimMontage>> Montages;

	// 몽타주 재생 속도
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float PlayRate = 1.0f;
	
private:
	// 태스크를 실행 중인 행동 트리 컴포넌트 참조
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<UBehaviorTreeComponent> CurrentOwnerComp;

	// 현재 실행 중인 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CurrentPlayingMontage;
	
	// 몽타주 종료 알림 핸들
	FOnMontageEnded MontageEndedDelegate;

};