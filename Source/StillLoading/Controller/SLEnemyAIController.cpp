// Fill out your copyright notice in the Description page of Project Settings.


#include "SLEnemyAIController.h"
#include "Perception/AISenseConfig_Sight.h"


ASLEnemyAIController::ASLEnemyAIController()
{
	AAIController::SetGenericTeamId(FGenericTeamId(2));
	AISenseConfig_Sight->SightRadius = 1600.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 30.f;
}

ETeamAttitude::Type ASLEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
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

void ASLEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

