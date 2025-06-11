#include "SLBTTask_PerformJumpAttack.h"
#include "AIController.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

USLBTTask_PerformJumpAttack::USLBTTask_PerformJumpAttack()
{
    NodeName = TEXT("Perform Jump Attack");
    bNotifyTick = true;
    bWaitForJumpAttackComplete = true;
    JumpAttackTimeout = 3.0f;
    TaskStartTime = 0.0f;
}

EBTNodeResult::Type USLBTTask_PerformJumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    ASLDoppelgangerCharacter* DoppelgangerCharacter = Cast<ASLDoppelgangerCharacter>(AIController->GetPawn());
    if (!DoppelgangerCharacter)
    {
        return EBTNodeResult::Failed;
    }

    // 점프 공격이 가능한지 확인
    if (!DoppelgangerCharacter->CanAIJump())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot perform jump attack - character cannot jump"));
        return EBTNodeResult::Failed;
    }

    // 점프 공격 실행
    DoppelgangerCharacter->PerformJumpAttack();
    
    TaskStartTime = GetWorld()->GetTimeSeconds();
    
    UE_LOG(LogTemp, Warning, TEXT("BTTask: Jump Attack Started"));

    // 완료까지 기다리지 않으면 즉시 성공 반환
    if (!bWaitForJumpAttackComplete)
    {
        return EBTNodeResult::Succeeded;
    }

    // 완료까지 기다리는 경우 InProgress 반환
    return EBTNodeResult::InProgress;
}

void USLBTTask_PerformJumpAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    ASLDoppelgangerCharacter* DoppelgangerCharacter = Cast<ASLDoppelgangerCharacter>(AIController->GetPawn());
    if (!DoppelgangerCharacter)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // 타임아웃 체크
    if (CurrentTime - TaskStartTime > JumpAttackTimeout)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask: Jump Attack Timeout"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 점프가 끝났고 착지했는지 확인
    if (!DoppelgangerCharacter->IsJumping() && !DoppelgangerCharacter->IsLanding())
    {
        // 공격 패턴이 None으로 돌아왔는지 확인 (공격 완료)
        if (DoppelgangerCharacter->GetCurrentActionPattern() == EDoppelgangerActionPattern::EDAP_None)
        {
            UE_LOG(LogTemp, Warning, TEXT("BTTask: Jump Attack Completed"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }
}