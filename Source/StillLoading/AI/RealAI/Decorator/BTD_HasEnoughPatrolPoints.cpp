#include "BTD_HasEnoughPatrolPoints.h"

#include "AIController.h"
#include "AI/RealAI/Boid/SwarmAgent.h"
#include "AI/RealAI/Boid/SwarmManager.h"

UBTD_HasEnoughPatrolPoints::UBTD_HasEnoughPatrolPoints()
{
	NodeName = TEXT("Has Enough Patrol Points");
}

bool UBTD_HasEnoughPatrolPoints::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	const ASwarmAgent* AICharacter = Cast<ASwarmAgent>(ControlledPawn);
	if (!AICharacter)
	{
		return false;
	}

	const ASwarmManager* SwarmManager = AICharacter->GetMySwarmManager();
	if (!SwarmManager)
	{
		return false;
	}

	const int32 PatrolPointsNum = AICharacter->MySwarmManager->SwarmPatrolPoints.Num();

	return PatrolPointsNum <= MinimumPatrolPoints;
}

FString UBTD_HasEnoughPatrolPoints::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Patrol Points <= %d"), *Super::GetStaticDescription(), MinimumPatrolPoints);
}