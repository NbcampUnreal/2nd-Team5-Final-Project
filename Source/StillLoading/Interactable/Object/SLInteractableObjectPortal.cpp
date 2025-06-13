// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectPortal.h"
#include "Kismet\GameplayStatics.h"
#include "Components\SphereComponent.h"
#include "Minigame/Object/SLObjectDestroyer.h"
#include "Components\ArrowComponent.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"

ASLInteractableObjectPortal::ASLInteractableObjectPortal()
{
    // GeometryCollectionComponent 생성
    GeometryCollectionComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComp"));
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    RootComponent = GeometryCollectionComp;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
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

    ObjectHp = 3;
}

void ASLInteractableObjectPortal::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = ArrowComponent->GetComponentLocation();
    SpawnRotation = ArrowComponent->GetComponentRotation();
    
    
}

void ASLInteractableObjectPortal::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
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
            BrokenDoor();
        }
    }
}

void ASLInteractableObjectPortal::BrokenDoor()
{
    ASLObjectDestroyer* BrokenActors = GetWorld()->SpawnActor<ASLObjectDestroyer>(DestroyerActor, SpawnLocation, SpawnRotation);
    if (BrokenActors)
    {
        BrokenActors->OnHitDoor.AddDynamic(this, &ASLInteractableObjectPortal::OnBroken);
    }
    

}

void ASLInteractableObjectPortal::OnBroken()
{

    GeometryCollectionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UE_LOG(LogTemp, Warning, TEXT("OnBroken"));
}
