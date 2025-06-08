#include "BTService_UpdateFormation.h"

#include "AIController.h"
#include "AI/RealAI/FormationComponent.h"

UBTService_UpdateFormation::UBTService_UpdateFormation()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
	bCreateNodeInstance = true;
	NodeName = TEXT("Update Formation Position");
}

void UBTService_UpdateFormation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return;

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn) return;

	UFormationComponent* FormationComponent = Pawn->FindComponentByClass<UFormationComponent>();
	if (!FormationComponent) return;

	FormationComponent->AssignStoredFormation(EFormationType::Wedge, FormationSpacing);
}