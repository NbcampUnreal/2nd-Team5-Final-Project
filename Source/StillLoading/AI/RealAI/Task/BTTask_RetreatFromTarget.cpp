#include "BTTask_RetreatFromTarget.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RetreatFromTarget::UBTTask_RetreatFromTarget()
{
    NodeName = TEXT("Retreat From Target");
}

EBTNodeResult::Type UBTTask_RetreatFromTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        return EBTNodeResult::Succeeded;
    }

    const float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());

    if (DistanceToTarget > MinimumDistance)
    {
        BlackboardComp->SetValueAsVector(RetreatLocation.SelectedKeyName, ControlledPawn->GetActorLocation());
        return EBTNodeResult::Succeeded;
    }

    const FVector MyLocation = ControlledPawn->GetActorLocation();
    const FVector TargetLocation = TargetActor->GetActorLocation();
    const FVector DirectionFromTarget = (MyLocation - TargetLocation).GetSafeNormal();

    const FVector RawRetreatLocation = MyLocation + DirectionFromTarget * FMath::RandRange(MinRetreatDistance, MaxRetreatDistance);

    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        return EBTNodeResult::Failed;
    }
    
    FNavLocation NavLocation;

    if (const bool bFoundNavLocation = NavSys->GetRandomPointInNavigableRadius(RawRetreatLocation, 100.0f, NavLocation))
    {
        BlackboardComp->SetValueAsVector(RetreatLocation.SelectedKeyName, NavLocation.Location);
        AIController->MoveToLocation(NavLocation.Location);
        
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

FString UBTTask_RetreatFromTarget::GetStaticDescription() const
{
    return FString::Printf(TEXT("Retreat if target is closer than %.1f units"), MinimumDistance);
}