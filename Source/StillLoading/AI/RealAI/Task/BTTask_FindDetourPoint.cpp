#include "BTTask_FindDetourPoint.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindDetourPoint::UBTTask_FindDetourPoint()
{
	NodeName = TEXT("Find Detour Point");
}

EBTNodeResult::Type UBTTask_FindDetourPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIOwner || !BlackboardComp) return EBTNodeResult::Failed;

	APawn* OwnerPawn = AIOwner->GetPawn();
	if (!OwnerPawn) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	// 1. 우회 방향 무작위 선택 (좌/우)
	const float DirectionMultiplier = FMath::RandBool() ? 1.0f : -1.0f;
	const FVector RightVector = OwnerPawn->GetActorRightVector() * DirectionMultiplier;

	// 2. 이상적인 우회 지점 계산
	const FVector IdealDetourPoint = OwnerPawn->GetActorLocation() + (RightVector * DetourDistance);

	// 3. 내비게이션 시스템을 통해 실제 이동 가능한 위치 탐색
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;

	FNavLocation ReachableLocation;
	const bool bFoundPoint = NavSystem->GetRandomReachablePointInRadius(IdealDetourPoint, SearchRadius, ReachableLocation);

	// 4. 결과 처리
	if (bFoundPoint)
	{
		// 찾은 위치를 블랙보드에 저장
		BlackboardComp->SetValueAsVector(DetourLocationKey.SelectedKeyName, ReachableLocation.Location);
		UE_LOG(LogTemp, Warning, TEXT("New Location [%s]"), *ReachableLocation.Location.ToString());
		return EBTNodeResult::Succeeded;
	}
    
	// 적절한 우회 지점을 찾지 못함
	return EBTNodeResult::Failed;
}