// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectPortal.h"
#include "Kismet\GameplayStatics.h"
#include "Chaos/ChaosEngineInterface.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection\GeometryCollectionComponent.h"
//#include "DestructibleComponent"

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
            FVector HitLocation = GetActorLocation(); // 파괴 중심 위치
            //float Radius = 100.0f; // 파괴 범위
            float Strain = 500.0f; // 응력 강도
            int32 PropagationDepth = 1; // 얼마나 깊이 퍼질지
            float PropagationFactor = 0.5f; // 응력 전파 감쇠율

            if (GeometryCollectionComp)
            {
                GeometryCollectionComp->ApplyInternalStrain(
                    0, // 첫 번째 조각부터
                    HitLocation,
                    Radius,
                    PropagationDepth,
                    PropagationFactor,
                    Strain
                );
            }
        }
    }
}
