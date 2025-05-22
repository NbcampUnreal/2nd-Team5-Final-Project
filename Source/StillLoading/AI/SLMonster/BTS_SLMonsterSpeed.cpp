// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SLMonster/BTS_SLMonsterSpeed.h"
#include "AIController.h"
#include "AI/SLMonster/SLMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"

UBTS_SLMonsterSpeed::UBTS_SLMonsterSpeed()
{
	NodeName = TEXT("Check Attack Condition");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
}

void UBTS_SLMonsterSpeed::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* AIPawn = AICon->GetPawn();
	ASLMonster* SLMon = Cast<ASLMonster>(AIPawn);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!Target)
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}

	TArray<AActor*> OverlappingActors;
	SLMon->AttackCheckSphere->GetOverlappingActors(OverlappingActors, ASLMonster::StaticClass());

	int32 AttackingCount = 0;
	for (AActor* Actor : OverlappingActors)
	{
		ASLMonster* OtherMonster = Cast<ASLMonster>(Actor);
		if (OtherMonster && OtherMonster->AttackSphere)
		{
			// 공격 중일 때만 AttackSphere 콜리전이 켜짐 → 그걸로 공격 상태 판단
			if (OtherMonster->AttackSphere->GetCollisionEnabled() == ECollisionEnabled::QueryOnly)
			{
				AttackingCount++;
			}
		}
	}

	const bool bCanAttack = AttackingCount < 1;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bCanAttack);

	/*float Distance = FVector::Dist(SLMon->GetActorLocation(), Target->GetActorLocation());

	if (Distance > AttackRange)
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}
	if (!SLMon->IsTargetInSight())
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}
	else
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), true);
		return;
	}*/
}