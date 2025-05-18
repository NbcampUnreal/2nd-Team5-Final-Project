// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectPortal.h"
#include "Kismet\GameplayStatics.h"
#include "GeometryCollection\GeometryCollectionComponent.h"
#include "Components\SphereComponent.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"
//#include "DestructibleComponent"

ASLReactiveObjectPortal::ASLReactiveObjectPortal()
{
    // GeometryCollectionComponent 생성
    GeometryCollectionComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComp"));

    RootComponent = GeometryCollectionComp;
    
    CollisionComp->SetupAttachment(GeometryCollectionComp);
    StaticMeshComp->SetupAttachment(GeometryCollectionComp);
    StaticMeshComp->SetHiddenInGame(true);
    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 시뮬레이션 설정
    GeometryCollectionComp->SetSimulatePhysics(true);
    GeometryCollectionComp->SetNotifyRigidBodyCollision(true);
    GeometryCollectionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GeometryCollectionComp->SetCollisionObjectType(ECC_PhysicsBody);

    CollisionComp->SetSimulatePhysics(false);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComp->SetCollisionObjectType(ECC_PhysicsBody);

    GeometryCollectionComp->SetSimulatePhysics(false);
    GeometryCollectionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

void ASLReactiveObjectPortal::BeginPlay()
{

    GeometryCollectionComp->SetSimulatePhysics(true);
}

void ASLReactiveObjectPortal::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
    if (InTriggerType == ESLReactiveTriggerType::ERT_InteractKey)
    {
        if (ObjectHp <= 0) return;
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
            StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            FTimerHandle CollisionOffTimer;
            GetWorld()->GetTimerManager().SetTimer(CollisionOffTimer,
                [this]
                {
                    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                },
                0.5f,
                false);
        }
    }
}
