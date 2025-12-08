// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectBase.h"

#include "NiagaraDataInterfaceSkeletalMesh.h"
#include "SLInteractableHighlight.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"

ASLInteractableObjectBase::ASLInteractableObjectBase()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCollisionProfileName("Interactable");
	SetRootComponent(StaticMeshComp);
	
	InteractableHighlight = CreateDefaultSubobject<USLInteractableHighlight>(TEXT("InteractableHighlight"));
	InteractableHighlight->SetupAttachment(StaticMeshComp);
	InteractableHighlight->PointLight->SetupAttachment(InteractableHighlight);
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
	InteractableHighlight->HighlightActivate();
}

void ASLInteractableObjectBase::OnUndetected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("OnUndetected"));
	InteractableHighlight->HighlightDeactivate();
}

void ASLInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASLInteractableObjectBase::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInteracted"));
	
	if (InteractionSound)
	{
		InteractableHighlight->SetInteracted(true);
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}
}

bool ASLInteractableObjectBase::IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType)
{
	if (TriggerType == ESLReactiveTriggerType::ERT_Both)
	{
		return true;
	}
	return InComingType == TriggerType;
}
