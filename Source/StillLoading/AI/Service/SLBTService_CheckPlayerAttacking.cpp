// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBTService_CheckPlayerAttacking.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"

USLBTService_CheckPlayerAttacking::USLBTService_CheckPlayerAttacking()
{
    NodeName = TEXT("Check Player Attacking");
    Interval = 0.1f;
    RandomDeviation = 0.05f;
    
    IsPlayerAttackingKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USLBTService_CheckPlayerAttacking, IsPlayerAttackingKey));
}

void USLBTService_CheckPlayerAttacking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
    float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return;
    }
    
    ASLBaseAIController* BaseAIController = Cast<ASLBaseAIController>(AIController);
    if (!BaseAIController)
    {
        return;
    }
    
    bool bIsPlayerAttacking = BaseAIController->IsPlayerAttacking();
    BlackboardComponent->SetValueAsBool(IsPlayerAttackingKey.SelectedKeyName, bIsPlayerAttacking);
}
