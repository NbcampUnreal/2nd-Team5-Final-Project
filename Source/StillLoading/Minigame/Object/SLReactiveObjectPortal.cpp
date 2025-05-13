// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectPortal.h"
#include "Kismet\GameplayStatics.h"
#include "Chaos/ChaosEngineInterface.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection\GeometryCollectionComponent.h"

ASLReactiveObjectPortal::ASLReactiveObjectPortal()
{
    // GeometryCollectionComponent 생성
    GeometryCollectionComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComp"));

    // 시뮬레이션 설정
    GeometryCollectionComp->SetSimulatePhysics(true);
    GeometryCollectionComp->SetNotifyRigidBodyCollision(true);
    GeometryCollectionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GeometryCollectionComp->SetCollisionObjectType(ECC_PhysicsBody);
}

void ASLReactiveObjectPortal::OnReacted(const ASLBaseCharacter*, ESLReactiveTriggerType InTriggerType)
{
    if (ObjectHp <= 0) return;
    if (InTriggerType == ESLReactiveTriggerType::ERT_InteractKey)
    {
        if (!DestinationLevelName.IsNone())
        {
            UGameplayStatics::OpenLevel(this, DestinationLevelName);
        }
    }
    else if (InTriggerType == ESLReactiveTriggerType::ERT_Hit)
    {
        if (ObjectHp > 0)
        {
            ObjectHp--;
        }
        else
        {

            GeometryCollectionComp->ApplyRadiusDamage(
                Strength,          // Damage
                Location,          // 위치
                Radius,            // Radius
                Strength * 10.f,   // Impulse
                false              // Full Damage
            );
            
        }
    }
}
