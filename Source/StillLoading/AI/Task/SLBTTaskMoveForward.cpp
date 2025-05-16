// Fill out your copyright notice in the Description page of Project Settings.


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
    
	// 지정된 시간 동안만 이동
	if (Memory->TimeElapsed <= MoveDuration)
	{
		// 캐릭터의 전방 벡터 방향으로 이동 입력 추가
		Character->AddMovementInput(Character->GetActorForwardVector(), MoveSpeed);
	}
	else
	{
		// 지정된 시간이 지나면 태스크 완료
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 USLBTTaskMoveForward::GetInstanceMemorySize() const
{
	return sizeof(FMoveForwardMemory);
}
