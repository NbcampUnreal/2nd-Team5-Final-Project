// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableBreakable.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Kismet/GameplayStatics.h"

ASLInteractableBreakable::ASLInteractableBreakable()
{
    BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
    StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
}

void ASLInteractableBreakable::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
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
    
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    OnObjectBreaked.Broadcast();
}

void ASLInteractableBreakable::BeginPlay()
{
    Super::BeginPlay();

    BattleComponent->OnCharacterHited.AddUniqueDynamic(this, &ThisClass::OnHited);
}

void ASLInteractableBreakable::OnHited(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
    if (ASLPlayerCharacterBase* Character = Cast<ASLPlayerCharacterBase>(DamageCauser))
    {
        TriggerReact(Character, ESLReactiveTriggerType::ERT_Hit);
        OnObjectHit.Broadcast(HitResult);
    }
}
