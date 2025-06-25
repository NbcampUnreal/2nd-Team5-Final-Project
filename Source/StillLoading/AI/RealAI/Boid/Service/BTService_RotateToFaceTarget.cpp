#include "BTService_RotateToFaceTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_RotateToFaceTarget::UBTService_RotateToFaceTarget()
{
	NodeName = "Rotate To Face Target";
	bNotifyTick = true;
}

void UBTService_RotateToFaceTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComp) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return;

	FVector Direction = TargetActor->GetActorLocation() - ControlledPawn->GetActorLocation();
	Direction.Z = 0.0f;
	const FRotator TargetRotation = Direction.Rotation();

	const FRotator NewRotation = FMath::RInterpTo(
		ControlledPawn->GetActorRotation(),
		TargetRotation,
		DeltaSeconds,
		InterpolationSpeed
	);

	ControlledPawn->SetActorRotation(NewRotation);
}