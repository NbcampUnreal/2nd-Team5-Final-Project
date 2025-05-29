#include "SLBTTaskMoveForward.h"

#include "AIController.h"
#include "GameFramework/Character.h"

USLBTTaskMoveForward::USLBTTaskMoveForward()
{
	NodeName = TEXT("Move Forward For Duration");
	bNotifyTick = true; 
}

EBTNodeResult::Type USLBTTaskMoveForward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FMoveForwardMemory* Memory = reinterpret_cast<FMoveForwardMemory*>(NodeMemory);
	Memory->TimeElapsed = 0.0f;
    
	return EBTNodeResult::InProgress;
}

void USLBTTaskMoveForward::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FMoveForwardMemory* Memory = reinterpret_cast<FMoveForwardMemory*>(NodeMemory);
	Memory->TimeElapsed += DeltaSeconds;
    
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
    
	ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
	if (!Character)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
    
	if (Memory->TimeElapsed <= MoveDuration)
	{
		FVector Start = Character->GetActorLocation();
		FVector Forward = Character->GetActorForwardVector();
		FVector End = Start + Forward * 100.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Character);

		FHitResult HitResult;
		bool bHit = Character->GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility,
			QueryParams
		);

		if (!bHit)
		{
			Character->AddMovementInput(Forward, MoveSpeed);
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 USLBTTaskMoveForward::GetInstanceMemorySize() const
{
	return sizeof(FMoveForwardMemory);
}
