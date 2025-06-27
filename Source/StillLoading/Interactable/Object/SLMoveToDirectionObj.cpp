// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Object/SLMoveToDirectionObj.h"
#include "Character/SLPlayerCharacterBase.h"

ASLMoveToDirectionObj::ASLMoveToDirectionObj() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASLMoveToDirectionObj::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);

	if (bIsActivated)
	{
		return;
	}

	FVector CharacterPos = InCharacter->GetActorLocation();
	StartLocation = GetActorLocation();

	MoveDirection = (StartLocation - CharacterPos);
	MoveDirection.Z = 0;
	MoveDirection = MoveDirection.GetSafeNormal();

	bIsActivated = true;
	SetActorTickEnabled(true);
}

void ASLMoveToDirectionObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FVector::Dist(GetActorLocation(), StartLocation) >= MoveDistance)
	{
		SetActorTickEnabled(false);
		bIsActivated = false;
	}
	else
	{
		SetActorLocation(GetActorLocation() + (MoveDirection * DeltaTime * MoveSpeed), false);
	}
}
