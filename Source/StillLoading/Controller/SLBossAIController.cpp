// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
ASLBossAIController::ASLBossAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	AAIController::SetGenericTeamId(FGenericTeamId(2));

	AISenseConfig_Sight->SightRadius = 4000.f; // 최대 감지 거리
	AISenseConfig_Sight->LoseSightRadius = 4200.f; // 감지 상실 거리
}

ETeamAttitude::Type ASLBossAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
	FGenericTeamId MyTeamID = GetGenericTeamId();

	// 팀 ID가 같으면 우호적
	if (OtherTeamID == MyTeamID)
	{
		return ETeamAttitude::Friendly;
	}
	else
	{
		return ETeamAttitude::Hostile;
	}
}

void ASLBossAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (Stimulus.WasSuccessfullySensed() && Actor)
		{
			if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
			{
				AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
                
				bool bShouldSetNewTarget = true;
                
				// 기존 타겟이 있으면 거리 비교
				if (CurrentTarget)
				{
					FVector MyLocation = GetPawn()->GetActorLocation();
                    
					// 기존 타겟과의 거리 계산
					float DistanceToCurrentTarget = FVector::Distance(MyLocation, CurrentTarget->GetActorLocation());
                    
					// 새 타겟과의 거리 계산
					float DistanceToNewTarget = FVector::Distance(MyLocation, Actor->GetActorLocation());
                    
					if (DistanceToNewTarget >= DistanceToCurrentTarget)
					{
						bShouldSetNewTarget = false;
					}
				}
                
				// 새 타겟으로 설정해야 하는 경우
				if (bShouldSetNewTarget)
				{
					TargetActor = Actor;
					BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
				}
			}
			else
			{
				GetBlackboardComponent()->ClearValue(FName("TargetActor"));
			}
		}
	}
}


