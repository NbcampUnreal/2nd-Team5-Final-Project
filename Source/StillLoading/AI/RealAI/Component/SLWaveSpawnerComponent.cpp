#include "SLWaveSpawnerComponent.h"

#include "GameFramework/Character.h"

USLWaveSpawnerComponent::USLWaveSpawnerComponent()
{
    PrimaryComponentTick.bCanEverTick = false; 
    CurrentWaveIndex = -1;
    bIsSpawningActive = false;
}

void USLWaveSpawnerComponent::BeginPlay()
{
    Super::BeginPlay();

    CachedOwnerSpawner = Cast<ASLSwarmSpawner>(GetOwner());
    if (!IsValid(CachedOwnerSpawner))
    {
        UE_LOG(LogTemp, Error, TEXT("USLWaveSpawnerComponent: 이 컴포넌트는 ASLSwarmSpawner 액터에 부착되어야 합니다!"));
    }
}

void USLWaveSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(DelayAfterWaveTimerHandle);
    }
    Super::EndPlay(EndPlayReason);
}

TArray<FSimpleSpawnComposition> USLWaveSpawnerComponent::GetAllWaveCompositions() const
{
    TArray<FSimpleSpawnComposition> AllCompositions;
    for (const FWaveData& WaveData : Waves)
    {
        for (const FWaveCompositionData& CompositionData : WaveData.WaveCompositions)
        {
            FSimpleSpawnComposition SimpleComp;
            SimpleComp.UnitClass = CompositionData.UnitClass;
            SimpleComp.SpawnCount = CompositionData.SpawnCount;

            AllCompositions.Add(SimpleComp);
        }
    }
    return AllCompositions;
}

bool USLWaveSpawnerComponent::StartWaveByIndex(int32 WaveIndex)
{
    if (!IsValid(CachedOwnerSpawner))
    {
        UE_LOG(LogTemp, Warning, TEXT("USLWaveSpawnerComponent: 소유 스포너(ASLSwarmSpawner)가 유효하지 않습니다. 웨이브를 시작할 수 없습니다."));
        return false;
    }

    if (bIsSpawningActive && CurrentWaveIndex == WaveIndex)
    {
        UE_LOG(LogTemp, Warning, TEXT("웨이브 %d는 이미 진행 중입니다."), WaveIndex);
        return false;
    }

    if (Waves.IsValidIndex(WaveIndex))
    {
        CurrentWaveIndex = WaveIndex;
        bIsSpawningActive = true;
        StartCurrentWaveSpawning();
        UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 웨이브 %d 시작 지시 받음."), *GetName(), WaveIndex);
        return true;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("USLWaveSpawnerComponent '%s': 유효하지 않은 웨이브 인덱스 (%d)입니다. 총 웨이브 수: %d"), *GetName(), WaveIndex, Waves.Num());
    return false;
}

void USLWaveSpawnerComponent::StartCurrentWaveSpawning()
{
    if (!Waves.IsValidIndex(CurrentWaveIndex) || !IsValid(CachedOwnerSpawner))
    {
        FinishCurrentWave();
        return;
    }

    const FWaveData& CurrentWaveData = Waves[CurrentWaveIndex];
    if (CurrentWaveData.WaveCompositions.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLWaveSpawnerComponent '%s': 웨이브 %d에 스폰할 유닛이 없습니다. 다음 웨이브로 넘어갑니다."), *GetName(), CurrentWaveIndex);
        FinishCurrentWave();
        return;
    }

    for (const FWaveCompositionData& Composition : CurrentWaveData.WaveCompositions)
    {
        for (int32 i = 0; i < Composition.SpawnCount; ++i)
        {
            if (ACharacter* SpawnedUnit = CachedOwnerSpawner->SpawnAndConfigureUnit(
                Composition.UnitClass,
                Composition.ControllerClass,
                Composition.TeamID,
                Composition.AvoidanceWeight)) {}
        }
    }

    UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 웨이브 %d 스폰 완료. 다음 웨이브까지 지연 시간: %.2f초."), *GetName(), CurrentWaveIndex, CurrentWaveData.DelayAfterWave);
    OnWaveCompleted.Broadcast(CurrentWaveIndex, this);

    if (CurrentWaveData.DelayAfterWave > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(DelayAfterWaveTimerHandle, this, &USLWaveSpawnerComponent::FinishCurrentWave, CurrentWaveData.DelayAfterWave, false);
    }
    else
    {
        FinishCurrentWave();
    }
}

void USLWaveSpawnerComponent::FinishCurrentWave()
{
    bIsSpawningActive = false;
    
    if (CurrentWaveIndex + 1 >= Waves.Num())
    {
        OnAllWavesCompleted.Broadcast(this);
        UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 모든 웨이브 최종 완료."), *GetName());
    }
}

void USLWaveSpawnerComponent::StopWaveSpawning()
{
    bIsSpawningActive = false;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(DelayAfterWaveTimerHandle);
    }
    UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 스폰 강제 중지."), *GetName());
}

bool USLWaveSpawnerComponent::IsWaveSpawningActive() const
{
    return bIsSpawningActive;
}
