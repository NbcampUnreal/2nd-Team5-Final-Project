// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Object/SLBothInteractableObjectBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

ASLBothInteractableObjectBase::ASLBothInteractableObjectBase() : Super()
{
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
	StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

    InteractionCollision = CreateDefaultSubobject<UBoxComponent>("InteractionCollision");
    InteractionCollision->SetupAttachment(StaticMeshComp);
    InteractionCollision->SetCollisionProfileName("Interactable");
    InteractionCollision->SetBoxExtent({ 100,100,100 });
}

void ASLBothInteractableObjectBase::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);

    if (CurrentHp < 0)
    {
        return;
    }

	switch (InTriggerType)
	{
	case ESLReactiveTriggerType::ERT_InteractKey:
        UE_LOG(LogTemp, Warning, TEXT("In Interaction"));
        OnInteraction.Broadcast();
		break;

	case ESLReactiveTriggerType::ERT_Hit:
		InComingAttack();
		break;
	}
}

void ASLBothInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();

	BattleComponent->OnCharacterHited.AddUniqueDynamic(this, &ThisClass::OnHited);
}

void ASLBothInteractableObjectBase::OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	if (ASLPlayerCharacterBase* Character = Cast<ASLPlayerCharacterBase>(DamageCauser))
	{
		TriggerReact(Character, ESLReactiveTriggerType::ERT_Hit);
		OnObjectHit.Broadcast(HitResult);
	}
}

void ASLBothInteractableObjectBase::InComingAttack()
{
    if (CurrentHp > 0)
    {
        if (InteractionSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
        }
        CurrentHp--;
        return;
    }

    if (DestroyEffect) // UNiagaraSystem* 변수
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            DestroyEffect,                    // Niagara System
            GetActorLocation(),           // 위치
            GetActorRotation(),           // 회전
            FVector(EffectScale),                // 스케일
            true,                         // AutoDestroy
            true,                         // AutoActivate
            ENCPoolMethod::None,          // Pooling 방식 (None/AutoRelease/ManualRelease)
            true                          // PreCullCheck
        );
    }

    if (DestroySound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
    }

    if (!bIsNotHidden)
    {
        SetActorHiddenInGame(true);
    }

    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    OnObjectBreaked.Broadcast();
}
