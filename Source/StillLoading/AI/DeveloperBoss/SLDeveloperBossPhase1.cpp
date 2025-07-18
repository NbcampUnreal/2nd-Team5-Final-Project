#include "SLDeveloperBossPhase1.h"
#include "SLDeveloperBoss.h"
#include "Character/SLAIBaseCharacter.h"
#include "LevelSequencePlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

ASLDeveloperBossPhase1::ASLDeveloperBossPhase1()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase1_BossRush;
    CurrentBossIndex = 0;
    TotalBossCount = 0;
}

void ASLDeveloperBossPhase1::BeginPlay()
{
    Super::BeginPlay();
}

void ASLDeveloperBossPhase1::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupDeadBosses();
    
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss))
        {
            UnregisterBossEvents(Boss);
            Boss->Destroy();
        }
    }
    SpawnedBosses.Empty();
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase1::StartPhase()
{
    Super::StartPhase();
    
    if (Config.BossClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1: No boss classes configured"));
        CompleteBossRush();
        return;
    }

    CurrentBossIndex = 0;
    TotalBossCount = Config.BossClasses.Num();
    SpawnedBosses.Empty();
    
    PlayStartCinematic();
}

void ASLDeveloperBossPhase1::EndPhase()
{
    CleanupDeadBosses();
    
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss))
        {
            UnregisterBossEvents(Boss);
            Boss->Destroy();
        }
    }
    SpawnedBosses.Empty();
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase1::IsPhaseCompleted() const
{
    return !bIsPhaseActive || (CurrentBossIndex >= Config.BossClasses.Num() && SpawnedBosses.Num() == 0);
}

void ASLDeveloperBossPhase1::SetConfig(const FSLPhase1Config& InConfig)
{
    Config = InConfig;
}

void ASLDeveloperBossPhase1::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (!bIsPhaseActive || !DeadBoss)
    {
        return;
    }
    
    UnregisterBossEvents(DeadBoss);
    SpawnedBosses.Remove(DeadBoss);

    if (IsValid(DeadBoss))
    {
        DeadBoss->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DeadBoss->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DeadBoss->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
        DeadBoss->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    }
    
    if (CurrentBossIndex < Config.BossClasses.Num())
    {
        PlayBossCinematic(CurrentBossIndex);
    }
    else
    {
        if (SpawnedBosses.Num() == 0)
        {
            CompleteBossRush();
        }
    }
}

void ASLDeveloperBossPhase1::SpawnNextBoss()
{
    if (!bIsPhaseActive || CurrentBossIndex >= Config.BossClasses.Num())
    {
        return;
    }

    TSubclassOf<ASLAIBaseCharacter> BossClass = Config.BossClasses[CurrentBossIndex];
    
    FVector SpawnLocation = GetActorLocation() + Config.BossSpawnOffset;
    SpawnLocation += FVector(FMath::RandRange(-100.f, 100.f), FMath::RandRange(-100.f, 100.f), 0.f);
    
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
    
    ASLAIBaseCharacter* SpawnedBoss = SpawnBossCharacter(BossClass, SpawnTransform);
    
    if (SpawnedBoss)
    {
        OnBossSpawnCompleted.Broadcast(SpawnedBoss);
        WeakenBoss(SpawnedBoss);
        CurrentBossIndex++;
    }
    else
    {
        CurrentBossIndex++;
    }
}

void ASLDeveloperBossPhase1::PlayStartCinematic()
{
    if (Config.Cinematics.IsValidIndex(0) && IsValid(Config.Cinematics[0]))
    {
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* SequenceActor = nullptr;
        ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            Config.Cinematics[0],
            PlaybackSettings,
            SequenceActor
        )->Play();
    }
    else
    {
        SpawnNextBoss();
    }
}

void ASLDeveloperBossPhase1::PlayBossCinematic(int32 BossIndex)
{
    int32 CinematicIndex = BossIndex + 1;
    
    if (Config.Cinematics.IsValidIndex(CinematicIndex) && IsValid(Config.Cinematics[CinematicIndex]))
    {
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* SequenceActor = nullptr;
        ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            Config.Cinematics[CinematicIndex],
            PlaybackSettings,
            SequenceActor
        )->Play();
    }
    else
    {
        SpawnNextBoss();
    }
}

int32 ASLDeveloperBossPhase1::GetBossesRemaining() const
{
    if (!bIsPhaseActive)
    {
        return 0;
    }
    
    int32 BossesToSpawn = Config.BossClasses.Num() - CurrentBossIndex;
    int32 SpawnedBossesAlive = SpawnedBosses.Num();
    
    return BossesToSpawn + SpawnedBossesAlive;
}

void ASLDeveloperBossPhase1::TestKillAllBosses()
{
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss) && !Boss->GetIsDead())
        {
            Boss->SetCurrentHealth(0.f);
            Boss->HandleDeath();
        }
    }
}

void ASLDeveloperBossPhase1::OnPhaseStarted()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 1 Boss Rush Started"));
}

void ASLDeveloperBossPhase1::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 1 Boss Rush Completed"));
}

void ASLDeveloperBossPhase1::WeakenBoss(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }

    float OriginalMaxHealth = Boss->GetMaxHealth();
    float WeakenedHealth = OriginalMaxHealth * Config.BossHealthMultiplier;
    
    Boss->SetMaxHealth(WeakenedHealth);
    Boss->SetCurrentHealth(WeakenedHealth);
    Boss->SetIsSpecialPattern(false);
}

void ASLDeveloperBossPhase1::CompleteBossRush()
{
    if (bIsPhaseActive)
    {
        EndPhase();
        CheckPhaseCompletion();
    }
}

void ASLDeveloperBossPhase1::RegisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.AddDynamic(this, &ASLDeveloperBossPhase1::HandleBossDeath);
}

void ASLDeveloperBossPhase1::UnregisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.RemoveAll(this);
}

void ASLDeveloperBossPhase1::CleanupDeadBosses()
{
    TArray<ASLAIBaseCharacter*> BossesToRemove;
    
    for (ASLAIBaseCharacter* Boss : SpawnedBosses)
    {
        if (IsValid(Boss) && Boss->GetIsDead())
        {
            BossesToRemove.Add(Boss);
        }
    }
    
    for (ASLAIBaseCharacter* DeadBoss : BossesToRemove)
    {
        SpawnedBosses.Remove(DeadBoss);
        UnregisterBossEvents(DeadBoss);
        
        if (IsValid(GetWorld()))
        {
            FTimerHandle DestroyTimer;
            GetWorld()->GetTimerManager().SetTimer(
                DestroyTimer,
                [DeadBoss]()
                {
                    if (IsValid(DeadBoss))
                    {
                        DeadBoss->Destroy();
                    }
                },
                2.0f,
                false
            );
        }
    }
}

ASLAIBaseCharacter* ASLDeveloperBossPhase1::SpawnBossCharacter(TSubclassOf<ASLAIBaseCharacter> BossClass, const FTransform& SpawnTransform)
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
    }
    
    return SpawnedBoss;
}