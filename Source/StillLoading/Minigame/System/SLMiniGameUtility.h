// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "EngineUtils.h"

// MiniGameManager 탐색 유틸리티
template<typename T>
T* FindActorInWorld(UWorld* World)
{
    for (TActorIterator<T> It(World); It; ++It)
    {
        return *It;
    }
    return nullptr;
}
