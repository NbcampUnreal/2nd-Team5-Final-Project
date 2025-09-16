#include "SLWaveSpawnerComponent.h"

#include "GameFramework/Character.h"

USLWaveSpawnerComponent::USLWaveSpawnerComponent()
{
    PrimaryComponentTick.bCanEverTick = false; 
    CurrentWaveIndex = -1;
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

    if (bEnableInfiniteRespawn)
    {
        if (WaveIndex != 0)
        {
            UE_LOG(LogTemp, Log, TEXT("무한 리스폰 모드: 첫 번째 웨이브만 진행합니다."));
            return false;
        }
        
        CurrentWaveIndex = 0;
        InitializeInfiniteRespawnMode();
        return true;
    }

    if (Waves.IsValidIndex(WaveIndex))
    {
        CurrentWaveIndex = WaveIndex;
        StartCurrentWaveSpawning();
        UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 웨이브 %d 시작 지시 받음."), *GetName(), WaveIndex);
        return true;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("USLWaveSpawnerComponent '%s': 유효하지 않은 웨이브 인덱스 (%d)입니다. 총 웨이브 수: %d"), *GetName(), WaveIndex, Waves.Num());
    return false;
}

void USLWaveSpawnerComponent::StartCurrentWaveSpawning()
{
    if (!Waves.IsValidIndex(CurrentWaveIndex) || !IsValid(CachedOwnerSpawner)) return;

    const FWaveData& CurrentWaveData = Waves[CurrentWaveIndex];
    if (CurrentWaveData.WaveCompositions.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLWaveSpawnerComponent '%s': 웨이브 %d에 스폰할 유닛이 없습니다. 다음 웨이브로 넘어갑니다."), *GetName(), CurrentWaveIndex);
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
}

void USLWaveSpawnerComponent::InitializeInfiniteRespawnMode()
{
    if (!Waves.IsValidIndex(0))
    {
        UE_LOG(LogTemp, Error, TEXT("무한 리스폰 모드: 첫 번째 웨이브가 없습니다!"));
        return;
    }

    const FWaveData& FirstWave = Waves[0];
    
    TargetUnitCounts.Empty();
    CurrentUnitCounts.Empty();
    UnitCompositionData.Empty();
    
    for (const FWaveCompositionData& Composition : FirstWave.WaveCompositions)
    {
        TargetUnitCounts.Add(Composition.UnitClass, Composition.SpawnCount);
        CurrentUnitCounts.Add(Composition.UnitClass, 0);
        UnitCompositionData.Add(Composition.UnitClass, Composition);
        
        for (int32 i = 0; i < Composition.SpawnCount; ++i)
        {
            if (ACharacter* SpawnedUnit = CachedOwnerSpawner->SpawnAndConfigureUnit(
                Composition.UnitClass,
                Composition.ControllerClass,
                Composition.TeamID,
                Composition.AvoidanceWeight))
            {
                CurrentUnitCounts[Composition.UnitClass]++;
            }
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        RespawnCheckTimerHandle,
        this,
        &USLWaveSpawnerComponent::CheckAndRespawnUnits,
        0.5f,
        true
    );
}

void USLWaveSpawnerComponent::CheckAndRespawnUnits()
{
    if (!bEnableInfiniteRespawn || !IsValid(CachedOwnerSpawner)) return;
    
    for (const auto& TargetPair : TargetUnitCounts)
    {
        TSubclassOf<ACharacter> UnitClass = TargetPair.Key;
        int32 TargetCount = TargetPair.Value;
        int32 CurrentCount = CurrentUnitCounts.FindRef(UnitClass);
        
        if (CurrentCount < TargetCount)
        {
            ImmediateRespawnFromPool(UnitClass);
        }
    }
}

void USLWaveSpawnerComponent::OnUnitReturnedToPool(TSubclassOf<ACharacter> UnitClass)
{
    if (!bEnableInfiniteRespawn) return;
    
    if (int32* CurrentCount = CurrentUnitCounts.Find(UnitClass))
    {
        *CurrentCount = FMath::Max(0, *CurrentCount - 1);
        
        FTimerHandle RespawnHandle;
        GetWorld()->GetTimerManager().SetTimer(
            RespawnHandle,
            [this, UnitClass]()
            {
                ImmediateRespawnFromPool(UnitClass);
            },
            RespawnDelay,
            false
        );
    }
}

void USLWaveSpawnerComponent::ImmediateRespawnFromPool(TSubclassOf<ACharacter> UnitClass)
{
    if (!bEnableInfiniteRespawn || !IsValid(CachedOwnerSpawner)) return;
    
    const FWaveCompositionData* CompositionData = UnitCompositionData.Find(UnitClass);
    if (!CompositionData) return;
    
    CachedOwnerSpawner->SpawnAndConfigureUnit(
            UnitClass, 
            CompositionData->ControllerClass, 
            CompositionData->TeamID, 
            CompositionData->AvoidanceWeight
        );
        
    CurrentUnitCounts[UnitClass]++;
}

void USLWaveSpawnerComponent::RespawnUnit(TSubclassOf<ACharacter> UnitClass, const FWaveCompositionData& CompositionData)
{
    if (!IsValid(CachedOwnerSpawner)) return;
    
    if (ACharacter* RespawnedUnit = CachedOwnerSpawner->SpawnAndConfigureUnit(
        CompositionData.UnitClass,
        CompositionData.ControllerClass,
        CompositionData.TeamID,
        CompositionData.AvoidanceWeight))
    {
        CurrentUnitCounts[UnitClass]++;
    }
}

void USLWaveSpawnerComponent::StopWaveSpawning()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(RespawnCheckTimerHandle); 
    }

    if (bEnableInfiniteRespawn)
    {
        TargetUnitCounts.Empty();
        CurrentUnitCounts.Empty();
        UnitCompositionData.Empty();
    }
    
    UE_LOG(LogTemp, Log, TEXT("USLWaveSpawnerComponent '%s': 스폰 강제 중지."), *GetName());
}
