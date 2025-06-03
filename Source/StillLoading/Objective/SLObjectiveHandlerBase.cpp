// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveHandlerBase.h"

USLObjectiveHandlerBase::USLObjectiveHandlerBase()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USLObjectiveHandlerBase::OnObjectiveStateChanged(ESLObjectiveState ObjectiveState)
{
	
}

void USLObjectiveHandlerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void USLObjectiveHandlerBase::SubscribeObjectives()
{
	
}
