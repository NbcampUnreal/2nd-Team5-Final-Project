#include "BTTask_MoveToAvailableCircleSlot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/RealAI/Blackboardkeys.h"

UBTTask_MoveToAvailableCircleSlot::UBTTask_MoveToAvailableCircleSlot()
{
	NodeName = "Move To Available Circle Slot";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToAvailableCircleSlot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToAvailableCircleSlot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	if (!AICon || !Pawn) return;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(BlackboardKeys::TargetActor));
	if (!Target) return;

	FVector Center = Target->GetActorLocation();
	UWorld* World = Pawn->GetWorld();
	if (!World) return;

	for (int32 i = 0; i < NumSlots; ++i)
	{
		float Angle = FMath::DegreesToRadians((360.f / NumSlots) * i);
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius;
		FVector ProbeLocation = Center + Offset;

		UE_LOG(LogTemp, Warning, TEXT("ProbeLocation[%s]"), *ProbeLocation.ToString());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Target);
		QueryParams.AddIgnoredActor(Pawn);
		FCollisionShape Sphere = FCollisionShape::MakeSphere(ProbeRadius);

		bool bBlocked = World->OverlapBlockingTestByChannel(ProbeLocation, FQuat::Identity, TraceChannel, Sphere,
		                                                    QueryParams);
		if (!bBlocked)
		{
			FColor DebugColor = bBlocked ? FColor::Red : FColor::Green;
			DrawDebugSphere(World, ProbeLocation, ProbeRadius, 12, DebugColor, false, UpdateInterval);
			DrawDebugLine(World, Center, ProbeLocation, DebugColor, false, UpdateInterval, 0, 2.0f);
			break;
		}

		AICon->MoveToLocation(ProbeLocation, AcceptanceRadius, true);
	}
}
