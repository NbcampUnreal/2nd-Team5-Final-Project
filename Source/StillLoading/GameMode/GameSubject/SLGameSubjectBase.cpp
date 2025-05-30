// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameSubjectBase.h"

#include "GameMode/SLGameModeBase.h"

USLGameSubjectBase::USLGameSubjectBase()
{
}

void USLGameSubjectBase::StartSubject()
{
	if (ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		SLGameMode->StartGameSubject(this);	
	}
}

void USLGameSubjectBase::EndSubject()
{
	if (ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		SLGameMode->EndGameSubject(this);
	}
}
