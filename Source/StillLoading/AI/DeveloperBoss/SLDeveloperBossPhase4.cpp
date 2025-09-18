#include "SLDeveloperBossPhase4.h"
#include "SLDeveloperBoss.h"
#include "AI/Actors/SLPhase4FallingFloor.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "LevelSequencePlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASLDeveloperBossPhase4::ASLDeveloperBossPhase4()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase4_Platformer;
    CurrentSequencePlayer = nullptr;
    bIsAutoWallAttackActive = false;
    bWaitingForCinematic = false;
    bIsCompleted = false;
}

void ASLDeveloperBossPhase4::BeginPlay()
{
    Super::BeginPlay();
}

void ASLDeveloperBossPhase4::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(GetWorld()) && CinematicTimeoutTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(CinematicTimeoutTimer);
    }
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    StopAutoWallAttack();
    
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase4::StartPhase()
{
    Super::StartPhase();
    
    bIsCompleted = false;
    bIsAutoWallAttackActive = false;
    bWaitingForCinematic = false;
    
    UE_LOG(LogTemp, Warning, TEXT("Phase4: Starting with start cinematic"));
    
    // Phase4 시작 시네마틱 재생
    PlayStartCinematic();
}

void ASLDeveloperBossPhase4::EndPhase()
{
    if (IsValid(GetWorld()) && CinematicTimeoutTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(CinematicTimeoutTimer);
    }
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    StopAutoWallAttack();
    
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase4::IsPhaseCompleted() const
{
    return bIsCompleted;
}

void ASLDeveloperBossPhase4::SetConfig(const FSLPhase4Config& InConfig)
{
    Config = InConfig;
    
    UE_LOG(LogTemp, Warning, TEXT("Phase4 Config Set:"));
    UE_LOG(LogTemp, Warning, TEXT("  - AutoWallAttackInterval: %f"), Config.AutoWallAttackInterval);
    UE_LOG(LogTemp, Warning, TEXT("  - InitialWallAttackDelay: %f"), Config.InitialWallAttackDelay);
    UE_LOG(LogTemp, Warning, TEXT("  - Cinematics: %d"), Config.Cinematics.Num());
    
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

void ASLDeveloperBossPhase4::StartAutoWallAttack()
{
    if (bIsAutoWallAttackActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: Auto wall attack already active"));
        return;
    }
    
    bIsAutoWallAttackActive = true;
    
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoWallAttackTimer,
            this,
            &ASLDeveloperBossPhase4::OnAutoWallAttackTimer,
            Config.InitialWallAttackDelay,
            false
        );
    }
}

void ASLDeveloperBossPhase4::StopAutoWallAttack()
{
    if (!bIsAutoWallAttackActive)
    {
        return;
    }
    
    bIsAutoWallAttackActive = false;
    
    if (IsValid(GetWorld()) && AutoWallAttackTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoWallAttackTimer);
    }
}

void ASLDeveloperBossPhase4::SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls)
{
    AvailableWalls.Empty();
    for (ASLLaunchableWall* Wall : InWalls)
    {
        if (IsValid(Wall))
        {
            AvailableWalls.Add(Wall);
        }
    }
}

void ASLDeveloperBossPhase4::PlayStartCinematic()
{
    UE_LOG(LogTemp, Warning, TEXT("Phase4: Playing start cinematic"));
    
    int32 CinematicIndex = 0; // 시작 시네마틱
    
    if (!Config.Cinematics.IsValidIndex(CinematicIndex))
    {
        UE_LOG(LogTemp, Error, TEXT(" Phase4: Invalid CinematicIndex: %d"), CinematicIndex);
        StartPhaseAfterCinematic(); // 시네마틱 없으면 바로 게임플레이 시작
        return;
    }
    
    if (!IsValid(Config.Cinematics[CinematicIndex]))
    {
        UE_LOG(LogTemp, Error, TEXT(" Phase4: Cinematic is null at index: %d"), CinematicIndex);
        StartPhaseAfterCinematic();
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase4: Starting cinematic: %s"), *Config.Cinematics[CinematicIndex]->GetName());
    
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
        UE_LOG(LogTemp, Display, TEXT("Phase4: SequencePlayer created successfully"));
        CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase4::OnCinematicFinished);
        CurrentSequencePlayer->Play();
        UE_LOG(LogTemp, Display, TEXT("Phase4: Cinematic play started"));
        
        // 타임아웃 설정
        if (IsValid(GetWorld()))
        {
            GetWorld()->GetTimerManager().SetTimer(
                CinematicTimeoutTimer,
                [this]()
                {
                    if (bWaitingForCinematic)
                    {
                        UE_LOG(LogTemp, Error, TEXT("Phase4: Cinematic timeout! Force finishing..."));
                        OnCinematicFinished();
                    }
                },
                10.0f,
                false
            );
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT(" Phase4: Failed to create SequencePlayer"));
        StartPhaseAfterCinematic();
    }
}

void ASLDeveloperBossPhase4::OnCinematicFinished()
{
    UE_LOG(LogTemp, Warning, TEXT(" Phase4: OnCinematicFinished called"));
    
    bWaitingForCinematic = false;
    
    if (IsValid(GetWorld()) && CinematicTimeoutTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(CinematicTimeoutTimer);
    }
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    StartPhaseAfterCinematic();
}

void ASLDeveloperBossPhase4::StartPhaseAfterCinematic()
{
    UE_LOG(LogTemp, Warning, TEXT("Phase4: Starting gameplay after start cinematic"));
    UE_LOG(LogTemp, Display, TEXT("️ Phase 4 Platformer Started"));
    
    // 시네마틱에서 바닥 붕괴가 처리되었으므로 바로 벽 공격 시작
    UE_LOG(LogTemp, Display, TEXT("Phase 4: Floor collapse handled by cinematic - Starting auto wall attacks"));
    StartAutoWallAttack();
    
    // 페이즈 완료 처리
    bIsCompleted = true;
    CheckPhaseCompletion();
}

void ASLDeveloperBossPhase4::OnPhaseStarted()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 4 Platformer Started"));
}

void ASLDeveloperBossPhase4::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 4 Platformer Completed"));
}

void ASLDeveloperBossPhase4::OnAutoWallAttackTimer()
{
    if (!bIsPhaseActive || !bIsAutoWallAttackActive)
    {
        return;
    }
    
    LaunchWallWithLines();
    
    // 다음 공격 예약
    if (bIsPhaseActive && bIsAutoWallAttackActive && IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoWallAttackTimer,
            this,
            &ASLDeveloperBossPhase4::OnAutoWallAttackTimer,
            Config.AutoWallAttackInterval,
            true  // 반복
        );
    }
}

void ASLDeveloperBossPhase4::LaunchWallWithLines()
{
    if (!bIsPhaseActive || !IsValid(OwnerBoss))
    {
        return;
    }
    
    // 발사 가능한 벽 찾기
    ASLLaunchableWall* TargetWall = nullptr;
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall) && Wall->CanLaunch())
        {
            TargetWall = Wall;
            break;
        }
    }
    
    if (IsValid(TargetWall))
    {
        UE_LOG(LogTemp, Display, TEXT("Phase4: Launching wall with immediate line activation: %s"), 
               *TargetWall->GetName());
        
        // 벽 발사
        TargetWall->LaunchWallToPlayer();
        
        // 즉시 선 활성화
        OwnerBoss->ActivateConnectedLines(PhaseIndex, TargetWall);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: No available walls to launch"));
    }
}