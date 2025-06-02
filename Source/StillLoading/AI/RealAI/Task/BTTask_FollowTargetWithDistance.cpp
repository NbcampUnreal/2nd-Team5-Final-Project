#include "BTTask_FollowTargetWithDistance.h"

#include "AIController.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FollowTargetWithDistance::UBTTask_FollowTargetWithDistance()
{
	NodeName = "Follow Target With Distance";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FollowTargetWithDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ElapsedTime = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_FollowTargetWithDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	if (ElapsedTime < UpdateInterval)
		return;

	ElapsedTime = 0.f;

	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	if (!AICon || !Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(BlackboardKeys::TargetActor));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector ToTarget = Target->GetActorLocation() - Pawn->GetActorLocation();
	ToTarget.Z = 0;
	float CurrentDistance = ToTarget.Size();

	if (CurrentDistance <= FollowDistance * 0.9f)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentDistance : %f [%f]"), CurrentDistance, FollowDistance);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FVector DesiredLocation = Target->GetActorLocation() - ToTarget.GetSafeNormal() * FollowDistance;
	AICon->MoveToLocation(DesiredLocation, AcceptanceRadius, true);

	FRotator LookAtRot = (Target->GetActorLocation() - Pawn->GetActorLocation()).Rotation();
	Pawn->SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
}