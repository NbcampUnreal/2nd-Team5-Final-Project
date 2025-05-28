#include "SLBTTask_Wander.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "AI/Testing/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

USLBTTask_Wander::USLBTTask_Wander()
{
	NodeName = TEXT("Wander");
}

EBTNodeResult::Type USLBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation ResultLocation;
	const float SearchRadius = 1000.f;
	const float MinDistanceFromOthers = 200.f;
	bool bFound = false;

	for (int i = 0; i < 10; ++i)
	{
		if (NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), SearchRadius, ResultLocation))
		{
			bool bTooClose = false;

			for (TActorIterator<AMonsterAICharacter> It(Pawn->GetWorld()); It; ++It)
			{
				if (*It == Pawn) continue;
				float DistSqr = FVector::DistSquared(It->GetActorLocation(), ResultLocation.Location);
				if (DistSqr < MinDistanceFromOthers * MinDistanceFromOthers)
				{
					bTooClose = true;
					break;
				}
			}

			if (!bTooClose)
			{
				bFound = true;
				break;
			}
		}
	}

	if (!bFound) return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("WanderLocation", ResultLocation.Location);
	return EBTNodeResult::Succeeded;
}
