#include "BTService_ScanForAllies.h"

#include "AIController.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_ScanForAllies::UBTService_ScanForAllies()
{
	NodeName = "Scan For Allies";
}

void UBTService_ScanForAllies::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!OwnerPawn) return;

	TArray<AActor*> AllPawns;
	UGameplayStatics::GetAllActorsOfClass(OwnerPawn->GetWorld(), OwnerPawn->GetClass(), AllPawns);

	int32 AllyCount = 0;
	for (AActor* Actor : AllPawns)
	{
		if (Actor != OwnerPawn && FVector::Dist(Actor->GetActorLocation(), OwnerPawn->GetActorLocation()) <= ScanRadius)
		{
			++AllyCount;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKeys::AllyCountKey, AllyCount);
}