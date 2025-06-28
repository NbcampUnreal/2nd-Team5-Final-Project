// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Object/SLMoveToDirectionObj.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/BattleComponent/BattleComponent.h"

ASLMoveToDirectionObj::ASLMoveToDirectionObj() : Super()
{
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
	StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	TriggerType = ESLReactiveTriggerType::ERT_Both;
}

void ASLMoveToDirectionObj::BeginPlay()
{
	Super::BeginPlay();
	BattleComponent->OnCharacterHited.AddUniqueDynamic(this, &ThisClass::OnHited);
}

void ASLMoveToDirectionObj::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);

	FVector CharacterPos = InCharacter->GetActorLocation();
	FVector StartLocation = GetActorLocation();

	FVector MoveDirection = (StartLocation - CharacterPos);
	MoveDirection.Z = 0;
	MoveDirection = MoveDirection.GetSafeNormal();

	if (InTriggerType == ESLReactiveTriggerType::ERT_InteractKey)
	{
		StaticMeshComp->AddImpulse(MoveDirection * MoveForce);
	}

	if (InTriggerType == ESLReactiveTriggerType::ERT_Hit)
	{
		StaticMeshComp->AddImpulse(MoveDirection * MoveForce * 1.5f);
	}
}

void ASLMoveToDirectionObj::OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	if (ASLPlayerCharacterBase* Character = Cast<ASLPlayerCharacterBase>(DamageCauser))
	{
		TriggerReact(Character, ESLReactiveTriggerType::ERT_Hit);
	}
}