#include "BTService_CheckArrival.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmAgent.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckArrival::UBTService_CheckArrival()
{
	NodeName = TEXT("Stop Rotate");
	
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_CheckArrival::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;
	ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(AIController->GetPawn());
	if (!OwningAgent) return;
	ASwarmManager* SwarmManager = OwningAgent->GetMySwarmManager();
	if (!SwarmManager) return;

	if (const AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(OwningAgent))
	{
		for (TObjectPtr<ASwarmAgent> SwarmAgent : SwarmManager->GetAgents())
		{
			SwarmAgent->StopSpinning();
		}
	}
}