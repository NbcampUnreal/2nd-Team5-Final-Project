#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "SLCompanionDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FMultiHitData
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FVector Location;
    
    UPROPERTY()
    float Radius;
    
    UPROPERTY()
    EAttackAnimType AttackType;
    
    UPROPERTY()
    int32 RemainingHits;
    
    UPROPERTY()
    float HitInterval;
    
    UPROPERTY()
    TArray<AActor*> HitActors;
    
    FMultiHitData()
    {
        Location = FVector::ZeroVector;
        Radius = 0.0f;
        AttackType = EAttackAnimType::AAT_Attack_01;
        RemainingHits = 0;
        HitInterval = 0.2f;
    }
};

namespace CompanionConstants
{
    // Flying
    constexpr float DEFAULT_FLYING_HEIGHT = 300.0f;
    constexpr float MAX_FLYING_HEIGHT = 800.0f;
    constexpr float MIN_FLYING_HEIGHT = 100.0f;
    constexpr float FLYING_SPEED = 200.0f;
    constexpr float HEIGHT_TOLERANCE = 5.0f;
    constexpr float FLYING_UPDATE_RATE = 0.02f;
    
    // Combat
    constexpr float KNOCKDOWN_DAMAGE_THRESHOLD = 20.0f;
    constexpr float LOW_HEALTH_THRESHOLD = 0.3f;
}