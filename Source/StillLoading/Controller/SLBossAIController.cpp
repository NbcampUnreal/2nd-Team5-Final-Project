// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
ASLBossAIController::ASLBossAIController()
{
	PrimaryActorTick.bCanEverTick = true;

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

void ASLBossAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAIController::SetGenericTeamId(FGenericTeamId(2));
}

void ASLBossAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	// 죽은 액터는 무시
	if (!IsActorAlive(Actor))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			// 부모 클래스의 타겟 시스템 사용
			AddOrUpdateTarget(Actor);
			UpdateTargetEvaluation();
		}
	}

}


