#include "SLDeveloperBoss.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLBossCharacter.h"
#include "Engine/World.h"

ASLDeveloperBoss::ASLDeveloperBoss()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASLDeveloperBoss::BeginPlay()
{
    Super::BeginPlay();
}

void ASLDeveloperBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DespawnAllBosses();
    Super::EndPlay(EndPlayReason);
}

ASLAIBaseCharacter* ASLDeveloperBoss::SpawnBossCharacter(TSubclassOf<ASLAIBaseCharacter> BossClass, const FTransform& SpawnTransform)
{
    if (!BossClass || !GetWorld())
    {
        return nullptr;
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    ASLAIBaseCharacter* SpawnedBoss = GetWorld()->SpawnActor<ASLAIBaseCharacter>(BossClass, SpawnTransform, SpawnParams);
    
    if (SpawnedBoss)
    {
        SpawnedBoss->SetIsSpecialPattern(true);
        SpawnedBosses.Add(SpawnedBoss);
        RegisterBossEvents(SpawnedBoss);
        
        UE_LOG(LogTemp, Display, TEXT("Developer Boss spawned: %s"), *SpawnedBoss->GetName());
    }
    
    return SpawnedBoss;
}

void ASLDeveloperBoss::DespawnAllBosses()
{
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss))
        {
            UnregisterBossEvents(Boss);
            Boss->Destroy();
        }
    }
    
    SpawnedBosses.Empty();
    
    OnDeveloperBossPatternFinished.Broadcast();
    
    UE_LOG(LogTemp, Display, TEXT("Developer Boss pattern finished - All bosses despawned"));
}

void ASLDeveloperBoss::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (DeadBoss)
    {
        OnBossCharacterDeath.Broadcast(DeadBoss);
        UnregisterBossEvents(DeadBoss);
        SpawnedBosses.Remove(DeadBoss);
    }
}

void ASLDeveloperBoss::HandlePatternFinished(ASLAIBaseCharacter* Boss)
{
    if (Boss)
    {
        OnBossPatternFinished.Broadcast(Boss);
    }
}

void ASLDeveloperBoss::RegisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.AddUObject(this, &ASLDeveloperBoss::HandleBossDeath);
    Boss->OnPatternFinished.AddUObject(this, &ASLDeveloperBoss::HandlePatternFinished);
}

void ASLDeveloperBoss::UnregisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.RemoveAll(this);
    Boss->OnPatternFinished.RemoveAll(this);
}

void ASLDeveloperBoss::TestSpawnRandomBoss()
{
    if (AvailableBossClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No boss classes available to spawn"));
        return;
    }
    
    // 랜덤 보스 선택
    int32 RandomIndex = FMath::RandRange(0, AvailableBossClasses.Num() - 1);
    TSubclassOf<ASLAIBaseCharacter> BossClass = AvailableBossClasses[RandomIndex];
    
    // 스폰 위치 계산 (개발자 보스 주변)
    FVector SpawnLocation = GetActorLocation() + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 100.f);
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
    
    ASLAIBaseCharacter* SpawnedBoss = SpawnBossCharacter(BossClass, SpawnTransform);
    
    if (SpawnedBoss)
    {
        UE_LOG(LogTemp, Display, TEXT("Test: Spawned %s at %s"), 
            *SpawnedBoss->GetClass()->GetName(), 
            *SpawnLocation.ToString());
    }
}

void ASLDeveloperBoss::TestSpawnAllBosses()
{
    float Radius = 500.f;
    int32 BossCount = AvailableBossClasses.Num();
    
    for (int32 i = 0; i < BossCount; i++)
    {
        float Angle = (360.f / BossCount) * i;
        float RadAngle = FMath::DegreesToRadians(Angle);
        
        FVector SpawnLocation = GetActorLocation() + FVector(
            FMath::Cos(RadAngle) * Radius,
            FMath::Sin(RadAngle) * Radius,
            100.f
        );
        
        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
        SpawnBossCharacter(AvailableBossClasses[i], SpawnTransform);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Test: Spawned all %d bosses"), BossCount);
}

void ASLDeveloperBoss::TestKillAllBosses()
{
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss) && !Boss->GetIsDead())
        {
            Boss->SetCurrentHealth(0.f);
            Boss->HandleDeath();
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Test: Killed all bosses"));
}