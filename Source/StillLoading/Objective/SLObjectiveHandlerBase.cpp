// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveHandlerBase.h"

#include "SLObjectiveBase.h"
#include "SLObjectiveSubsystem.h"

USLObjectiveHandlerBase::USLObjectiveHandlerBase()
{
	PrimaryComponentTick.bCanEverTick = true;

}

USLObjectiveBase* USLObjectiveHandlerBase::GetObjective()
{
	USLObjectiveSubsystem *ObjectiveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
	if (ObjectiveSubsystem == nullptr)
	{
		return nullptr;
	}

	USLObjectiveBase* Objective =  ObjectiveSubsystem->GetObjective(Chapter, ObjectiveName);
	return Objective;
}

void USLObjectiveHandlerBase::OnObjectiveStateChanged(const ESLObjectiveState ObjectiveState)
{
	switch (ObjectiveState)
	{
		case ESLObjectiveState::InProgress:
			OnObjectiveInProgressed();
		break;
		case ESLObjectiveState::Complete:
			OnObjectiveCompleted();
		break;
		case ESLObjectiveState::Fail:
			OnObjectiveFailed();
		break;
		case ESLObjectiveState::Standby:
			OnObjectiveStandby();
		break;
		default:
			break;
	}
}


void USLObjectiveHandlerBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeObjective();
}

void USLObjectiveHandlerBase::OnObjectiveProgressUpdated_Implementation(const int32 ProgressCount)
{
	OnObjectiveProgressUpdatedDelegate.Broadcast(ProgressCount);
}

void USLObjectiveHandlerBase::OnObjectiveCompleted_Implementation()
{
	OnObjectiveCompletedDelegate.Broadcast(this);
}

void USLObjectiveHandlerBase::OnObjectiveInProgressed_Implementation()
{
	OnObjectiveInProgressedDelegate.Broadcast(this);
}

void USLObjectiveHandlerBase::OnObjectiveFailed_Implementation()
{
	OnObjectiveFailedDelegate.Broadcast(this);
}

void USLObjectiveHandlerBase::OnObjectiveStandby_Implementation()
{
	OnObjectiveStandbyDelegate.Broadcast(this);
}

void USLObjectiveHandlerBase::InitializeObjective()
{
	USLObjectiveBase* Objective = GetObjective();
	if (Objective == nullptr)
	{
		return;
	}
	Objective->OnObjectiveStateChanged.AddDynamic(this, &USLObjectiveHandlerBase::OnObjectiveStateChanged);
	Objective->OnObjectiveProgressChanged.AddDynamic(this, &USLObjectiveHandlerBase::OnObjectiveProgressUpdated);

	OnObjectiveStateChanged(Objective->GetObjectiveState());
	OnObjectiveProgressUpdated(Objective->GetObjectiveProgressCount());
}
