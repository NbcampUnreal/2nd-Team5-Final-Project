// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectBase.h"

#include "StillLoading\Character\SLPlayerCharacterBase.h"

ASLInteractableObjectBase::ASLInteractableObjectBase()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCollisionProfileName("RadarDetectable");
	SetRootComponent(StaticMeshComp);
}

void ASLInteractableObjectBase::TriggerReact(ASLPlayerCharacterBase* InCharacter, const ESLReactiveTriggerType InComingType)
{
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not IsValid"));
		return;
	}
	
	if (!IsTriggerTypeAllowed(InComingType))
	{
		UE_LOG(LogTemp, Warning, TEXT("Type Failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnReacted"));
	OnInteracted(InCharacter, InComingType);
}

void ASLInteractableObjectBase::OnDetected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDetected"));
}

void ASLInteractableObjectBase::OnUndetected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("OnUndetected"));
}

void ASLInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASLInteractableObjectBase::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInteracted"));
}

bool ASLInteractableObjectBase::IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType)
{
	if (TriggerType == ESLReactiveTriggerType::ERT_Both)
	{
		return true;
	}
	return InComingType == TriggerType;
}
