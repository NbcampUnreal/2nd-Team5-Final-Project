// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SLMonster/BTTask_SLMonsterSetSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_SLMonsterSetSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
	if (!Character) return EBTNodeResult::Failed;

	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;

	return EBTNodeResult::Succeeded;
}