#include "SLBTTask_MoveToWithRotation.h"

#include "AIController.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "Navigation/PathFollowingComponent.h"

USLBTTask_MoveToWithRotation::USLBTTask_MoveToWithRotation()
{
	NodeName = TEXT("Move To With Rotation");
	bNotifyTick = true;
}

EBTNodeResult::Type USLBTTask_MoveToWithRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TimeElapsed = 0.f;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AMonsterAICharacter* ControlledPawn = Cast<AMonsterAICharacter>(AICon->GetPawn());
	if (!ControlledPawn) return EBTNodeResult::Failed;
	
	if (ControlledPawn->IsLeader() && bLeaderOnly)
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		if (!BBComp) return EBTNodeResult::Failed;

		AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(BlackboardKeys::TargetActor));
		if (!Target) return EBTNodeResult::Failed;

		FAIMoveRequest MoveReq;
		MoveReq.SetGoalActor(Target);
		MoveReq.SetAcceptanceRadius(10.0f);
		AICon->MoveTo(MoveReq);
	}
	else
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void USLBTTask_MoveToWithRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bLeaderOnly)
	{
		TimeElapsed += DeltaSeconds;

		AAIController* AICon = OwnerComp.GetAIOwner();
		APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;

		if (!Pawn || !AICon)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		FVector MoveDirection = AICon->GetImmediateMoveDestination() - Pawn->GetActorLocation();
		MoveDirection.Z = 0.f;

		if (!MoveDirection.IsNearlyZero())
		{
			FRotator TargetRotation = MoveDirection.Rotation();
			FRotator NewRotation = FMath::RInterpTo(Pawn->GetActorRotation(), TargetRotation, DeltaSeconds, RotationSpeed);
			Pawn->SetActorRotation(NewRotation);
		}

		if (TimeElapsed >= MaxMoveDuration)
		{
			AICon->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
