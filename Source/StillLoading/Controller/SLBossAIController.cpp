// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"


// Sets default values
ASLBossAIController::ASLBossAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	AAIController::SetGenericTeamId(FGenericTeamId(2));
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
				TargetActor = Actor;

				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			
			}
			else
			{

				GetBlackboardComponent()->ClearValue(FName("TargetActor"));
			}
		}
	}
}


