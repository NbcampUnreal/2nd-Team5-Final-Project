// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_BackStep.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_BackStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;

	if (Pawn)
	{
		FVector BackwardDirection = -Pawn->GetActorForwardVector();
		FVector NewLocation = Pawn->GetActorLocation() + BackwardDirection * Distance;

		AIController->MoveToLocation(NewLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}