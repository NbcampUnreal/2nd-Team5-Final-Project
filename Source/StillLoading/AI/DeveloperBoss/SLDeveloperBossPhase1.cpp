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

    CurrentSequencePlayer = nullptr;
    bWaitingForCinematic = false;
    PendingBossIndex = -1;
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
    bWaitingForCinematic = false;
    PendingBossIndex = -1;
}

void ASLDeveloperBossPhase1::EndPhase()
{
    UE_LOG(LogTemp, Warning, TEXT("Phase1 EndPhase called"));
    
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    CleanupDeadBosses();
    
    TArray<ASLAIBaseCharacter*> BossesToCleanup = SpawnedBosses;
    SpawnedBosses.Empty();
    
    for (ASLAIBaseCharacter* Boss : BossesToCleanup)
    {
        if (IsValid(Boss))
        {
            UnregisterBossEvents(Boss);
            Boss->Destroy();
        }
    }
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase1::IsPhaseCompleted() const
{
    return !bIsPhaseActive || (CurrentBossIndex >= Config.BossClasses.Num() && SpawnedBosses.Num() == 0);
}

void ASLDeveloperBossPhase1::SetConfig(const FSLPhase1Config& InConfig)
{
    Config = InConfig;
    
    for (int32 i = 0; i < Config.Cinematics.Num(); i++)
    {
        if (IsValid(Config.Cinematics[i]))
        {
            UE_LOG(LogTemp, Warning, TEXT("  - Cinematic[%d]: %s"), i, *Config.Cinematics[i]->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("  - Cinematic[%d]: NULL"), i);
        }
    }
}

void ASLDeveloperBossPhase1::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (!bIsPhaseActive || !IsValid(DeadBoss) || bWaitingForCinematic)
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleBossDeath: Invalid conditions"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Boss death detected: %s"), *DeadBoss->GetName());
    
    // 1. 먼저 이벤트 바인딩 해제 (안전한 상태에서)
    if (IsValid(DeadBoss))
    {
        UnregisterBossEvents(DeadBoss);
    }
    
    // 2. 배열에서 제거
    SpawnedBosses.RemoveSingle(DeadBoss);
    
    // 3. 콜리전 비활성화 (안전 체크 포함)
    if (IsValid(DeadBoss))
    {
        if (UPrimitiveComponent* CapsuleComp = DeadBoss->GetCapsuleComponent())
        {
            CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
        
        if (USkeletalMeshComponent* MeshComp = DeadBoss->GetMesh())
        {
            MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
    }
    
    // 4. 딜레이를 두고 다음 처리 (메모리 안전성 확보)
    if (IsValid(GetWorld()))
    {
        FTimerHandle DeathProcessTimer;
        GetWorld()->GetTimerManager().SetTimer(
            DeathProcessTimer,
            [this]()
            {
                ProcessNextBossAfterDeath();
            },
            0.1f, // 짧은 딜레이
            false
        );
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
    if (!IsValid(Boss))
    {
        UE_LOG(LogTemp, Warning, TEXT("RegisterBossEvents: Boss is invalid"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Registering events for boss: %s"), *Boss->GetName());
    
    // 중복 바인딩 방지
    Boss->OnCharacterDeath.RemoveAll(this);
    Boss->OnCharacterDeath.AddDynamic(this, &ASLDeveloperBossPhase1::HandleBossDeath);
}

void ASLDeveloperBossPhase1::UnregisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!IsValid(Boss))
    {
        UE_LOG(LogTemp, Warning, TEXT("UnregisterBossEvents: Boss is invalid"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Unregistering events for boss: %s"), *Boss->GetName());
    
    // 안전하게 이벤트 바인딩 해제
    if (Boss->OnCharacterDeath.IsBound())
    {
        Boss->OnCharacterDeath.RemoveAll(this);
    }
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
        UE_LOG(LogTemp, Error, TEXT("SpawnBossCharacter: Invalid class or world"));
        return nullptr;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Spawning boss: %s"), *BossClass->GetName());
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    ASLAIBaseCharacter* SpawnedBoss = GetWorld()->SpawnActor<ASLAIBaseCharacter>(BossClass, SpawnTransform, SpawnParams);
    
    if (IsValid(SpawnedBoss))
    {
        UE_LOG(LogTemp, Display, TEXT("Boss spawned successfully: %s"), *SpawnedBoss->GetName());
        
        SpawnedBoss->SetIsSpecialPattern(true);
        SpawnedBosses.Add(SpawnedBoss);
        RegisterBossEvents(SpawnedBoss);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn boss"));
    }
    
    return SpawnedBoss;
}

void ASLDeveloperBossPhase1::HandleLineDestroyed(int32 LineIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Phase1::HandleLineDestroyed - LineIndex: %d"), LineIndex);
    
    Super::HandleLineDestroyed(LineIndex);
    
    if (!bIsPhaseActive)
    {
        UE_LOG(LogTemp, Error, TEXT("Phase1 is not active!"));
        return;
    }
    
    if (bWaitingForCinematic)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already waiting for cinematic"));
        return;
    }
    
    int32 DestroyedCount = 0;
    if (IsValid(OwnerBoss))
    {
        DestroyedCount = OwnerBoss->GetCurrentPhaseDestroyedLinesCount();
        UE_LOG(LogTemp, Warning, TEXT("DestroyedCount: %d"), DestroyedCount);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerBoss is invalid!"));
        return;
    }
    
    PlayCinematicForLineDestroy(DestroyedCount);
}

void ASLDeveloperBossPhase1::PlayCinematicForLineDestroy(int32 DestroyedLineCount)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayCinematicForLineDestroy - Count: %d"), DestroyedLineCount);
    
    int32 CinematicIndex = 0;
    PendingBossIndex = DestroyedLineCount - 1;
    
    UE_LOG(LogTemp, Warning, TEXT("CinematicIndex: %d, PendingBossIndex: %d"), CinematicIndex, PendingBossIndex);
    UE_LOG(LogTemp, Warning, TEXT("Config.Cinematics.Num(): %d"), Config.Cinematics.Num());
    
    if (!Config.Cinematics.IsValidIndex(CinematicIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid CinematicIndex: %d"), CinematicIndex);
        OnCinematicFinished(); // 시네마틱 없으면 바로 보스 스폰
        return;
    }
    
    if (!IsValid(Config.Cinematics[CinematicIndex]))
    {
        UE_LOG(LogTemp, Error, TEXT("Cinematic is null at index: %d"), CinematicIndex);
        OnCinematicFinished();
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Starting cinematic: %s"), *Config.Cinematics[CinematicIndex]->GetName());
    
    bWaitingForCinematic = true;
    
    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    ALevelSequenceActor* SequenceActor = nullptr;
    CurrentSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        GetWorld(),
        Config.Cinematics[CinematicIndex],
        PlaybackSettings,
        SequenceActor
    );
    
    if (CurrentSequencePlayer)
    {
        UE_LOG(LogTemp, Display, TEXT(" SequencePlayer created successfully"));
        CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase1::OnCinematicFinished);
        CurrentSequencePlayer->Play();
        UE_LOG(LogTemp, Display, TEXT(" Cinematic play started"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT(" Failed to create SequencePlayer"));
        OnCinematicFinished();
    }
}

void ASLDeveloperBossPhase1::PlayCinematicForBossDeath(int32 DeadBossIndex)
{
    // 보스 죽음 시네마틱 (인덱스 DeadBossIndex + 1)
    int32 CinematicIndex = DeadBossIndex + 1;
    PendingBossIndex = CurrentBossIndex; // 다음 스폰할 보스 인덱스
    
    if (Config.Cinematics.IsValidIndex(CinematicIndex) && IsValid(Config.Cinematics[CinematicIndex]))
    {
        bWaitingForCinematic = true;
        
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* SequenceActor = nullptr;
        CurrentSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            Config.Cinematics[CinematicIndex],
            PlaybackSettings,
            SequenceActor
        );
        
        if (CurrentSequencePlayer)
        {
            CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase1::OnCinematicFinished);
            CurrentSequencePlayer->Play();
        }
        else
        {
            OnCinematicFinished();
        }
    }
    else
    {
        // 시네마틱이 없으면 바로 다음 보스 스폰
        SpawnNextBoss();
    }
}

void ASLDeveloperBossPhase1::OnCinematicFinished()
{
    bWaitingForCinematic = false;
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    // 시네마틱 완료 후 보스 스폰
    if (PendingBossIndex >= 0 && PendingBossIndex < Config.BossClasses.Num())
    {
        if (CurrentBossIndex <= PendingBossIndex)
        {
            CurrentBossIndex = PendingBossIndex;
            SpawnNextBoss();
        }
    }
    
    PendingBossIndex = -1;
}

void ASLDeveloperBossPhase1::ProcessNextBossAfterDeath()
{
    if (!bIsPhaseActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Processing next boss after death"));
    
    // 보스 죽음에 대한 시네마틱 실행
    if (CurrentBossIndex < Config.BossClasses.Num())
    {
        PlayCinematicForBossDeath(CurrentBossIndex - 1);
    }
    else
    {
        if (SpawnedBosses.Num() == 0)
        {
            CompleteBossRush();
        }
    }
}