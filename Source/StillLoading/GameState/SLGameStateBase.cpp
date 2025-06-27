// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameStateBase.h"

ASLGameStateBase::ASLGameStateBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

TArray<TObjectPtr<USLObjectiveBase>>& ASLGameStateBase::GetInProgressedObjectives()
{
	return InProgressedObjectives;
}

TArray<TObjectPtr<USLObjectiveBase>>& ASLGameStateBase::GetModifedObjectives()
{
	return ModifiedObjectives;
}

void ASLGameStateBase::BeginPlay()
{
	Super::BeginPlay();

}
