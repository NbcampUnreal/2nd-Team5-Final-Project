// Fill out your copyright notice in the Description page of Project Settings.


#include "Geometry/SLGeometryCollectionActor.h"

#include "Geometry/SLGeometryManagerSubsystem.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "TimerManager.h"


ASLGeometryCollectionActor::ASLGeometryCollectionActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASLGeometryCollectionActor::BeginPlay()
{
    Super::BeginPlay();

    // 매니저에 자동 등록
    if (USLGeometryManagerSubsystem* Manager = GetWorld()->GetSubsystem<USLGeometryManagerSubsystem>())
    {
        Manager->RegisterGeometryCollection(this);
    }

}

void ASLGeometryCollectionActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (USLGeometryManagerSubsystem* Manager = GetWorld()->GetSubsystem<USLGeometryManagerSubsystem>())
    {
        Manager->UnregisterGeometryCollection(this);
    }

    Super::EndPlay(EndPlayReason);
}

