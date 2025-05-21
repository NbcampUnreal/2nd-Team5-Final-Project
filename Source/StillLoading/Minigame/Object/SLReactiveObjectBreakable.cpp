// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectBreakable.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void ASLReactiveObjectBreakable::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
    if (CurrentHp > 0)
    {
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
            FVector(1.0f),                // 스케일
            true,                         // AutoDestroy
            true,                         // AutoActivate
            ENCPoolMethod::None,          // Pooling 방식 (None/AutoRelease/ManualRelease)
            true                          // PreCullCheck
        );
    }
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}
