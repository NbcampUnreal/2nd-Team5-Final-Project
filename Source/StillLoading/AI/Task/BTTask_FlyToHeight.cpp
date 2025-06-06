// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_FlyToHeight.h"

#include "AIController.h"
#include "AI/Companion/SLCompanionCharacter.h"
#include "AI/Components/SLCompanionFlyingComponent.h"

UBTTask_FlyToHeight::UBTTask_FlyToHeight()
{
    NodeName = "Fly To Height";
    bNotifyTick = true;
    
    // 기본값 설정
    TargetHeight = 300.0f;
    Speed = 200.0f;
    AcceptableRadius = 20.0f;
    bStopOnFail = true;
}

EBTNodeResult::Type UBTTask_FlyToHeight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }
    
    ASLCompanionCharacter* Companion = Cast<ASLCompanionCharacter>(AIController->GetPawn());
    if (!Companion)
    {
        return EBTNodeResult::Failed;
    }
    
    USLCompanionFlyingComponent* FlyingComponent = Companion->GetFlyingComponent();
    if (!FlyingComponent)
    {
        return EBTNodeResult::Failed;
    }
    
    // 이미 목표 높이에 있는지 확인
    if (IsHeightReached(Companion))
    {
        return EBTNodeResult::Succeeded;
    }
    
    // 비행 시작
    FlyingComponent->FlyToHeight(TargetHeight, Speed);
    
    // 비동기 실행을 위해 InProgress 반환
    return EBTNodeResult::InProgress;
}

void UBTTask_FlyToHeight::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    ASLCompanionCharacter* Companion = Cast<ASLCompanionCharacter>(AIController->GetPawn());
    if (!Companion || !Companion->GetFlyingComponent())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    // 목표 도달 확인
    if (IsHeightReached(Companion))
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_FlyToHeight: Reached target height"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    // 비행 컴포넌트가 비활성화되었는지 확인
    USLCompanionFlyingComponent* FlyingComponent = Companion->GetFlyingComponent();
    if (TargetHeight > 0 && !FlyingComponent->IsFlying())
    {
        // 비행해야 하는데 비행 중이 아님
        if (bStopOnFail)
        {
            UE_LOG(LogTemp, Warning, TEXT("BTTask_FlyToHeight: Flying was interrupted"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        }
        else
        {
            // 다시 시도
            FlyingComponent->FlyToHeight(TargetHeight, Speed);
        }
    }
}

bool UBTTask_FlyToHeight::IsHeightReached(ASLCompanionCharacter* Companion) const
{
    if (!Companion || !Companion->GetFlyingComponent())
    {
        return false;
    }
    
    float CurrentHeight = Companion->GetFlyingComponent()->GetCurrentHeight();
    float HeightDifference = FMath::Abs(CurrentHeight - TargetHeight);
    
    return HeightDifference <= AcceptableRadius;
}

FString UBTTask_FlyToHeight::GetStaticDescription() const
{
    return FString::Printf(TEXT("Fly to height: %.1f units at %.1f speed\nAcceptable radius: %.1f"), 
        TargetHeight, Speed, AcceptableRadius);
}