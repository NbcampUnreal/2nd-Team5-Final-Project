// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLPlayMontageTask.generated.h"

class UAnimInstance;
class UBehaviorTreeComponent;

/**
 * 가변 속도로 애니메이션 몽타주를 재생하기 위한 BT Task
 */
UCLASS()
class STILLLOADING_API USLPlayMontageTask : public UBTTaskNode
{
    GENERATED_BODY()

public:
    USLPlayMontageTask();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    /** 몽타주 완료를 기다릴지 여부 */
    UPROPERTY(EditAnywhere, Category = "Montage")
    bool bWaitForMontageToFinish;

    /** 재생할 몽타주 */
    UPROPERTY(EditAnywhere, Category = "Montage")
    TObjectPtr<UAnimMontage> MontageToPlay;

    /** 몽타주 재생 속도 (1.0 = 일반 속도) */
    UPROPERTY(EditAnywhere, Category = "Montage", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float PlayRate;

    /** 블렌드 인 시간 */
    UPROPERTY(EditAnywhere, Category = "Montage", meta = (ClampMin = "0.0"))
    float BlendInTime;

    /** 시작할 섹션 이름 */
    UPROPERTY(EditAnywhere, Category = "Montage")
    FName SectionName;

private:
    /** 몽타주 종료 알림 함수 */
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    /** 현재 재생 중인 몽타주 참조 */
    UPROPERTY()
    TObjectPtr<UAnimMontage> CurrentMontage;

    /** 비헤이비어 트리 컴포넌트 참조 */
    UPROPERTY()
    TObjectPtr<UBehaviorTreeComponent> BTComp;

    /** 애님 인스턴스 참조 */
    UPROPERTY()
    TObjectPtr<UAnimInstance> AnimInstance;
};