// Fill out your copyright notice in the Description page of Project Settings.


#include "SLNPCAIController.h"


ASLNPCAIController::ASLNPCAIController()
{
	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type ASLNPCAIController::GetTeamAttitudeTowards(const AActor& Other) const
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

void ASLNPCAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

