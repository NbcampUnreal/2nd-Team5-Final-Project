#include "SLDeveloperBossPhase4.h"
#include "SLDeveloperBoss.h"
#include "AI/Actors/SLPhase4FallingFloor.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASLDeveloperBossPhase4::ASLDeveloperBossPhase4()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase4_Platformer;
    FallingFloor = nullptr;
    bIsAutoWallAttackActive = false;
    bIsCompleted = false;
}

void ASLDeveloperBossPhase4::BeginPlay()
{
    Super::BeginPlay();
    
    // Config에서 FallingFloor 설정
    if (Config.FallingFloor)
    {
        SetFallingFloor(Config.FallingFloor);
    }
}

void ASLDeveloperBossPhase4::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopAutoWallAttack();
    
    if (IsValid(FallingFloor))
    {
        FallingFloor->OnFloorCollapseCompleted.RemoveAll(this);
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase4::StartPhase()
{
    Super::StartPhase();
    
    bIsCompleted = false;
    bIsAutoWallAttackActive = false;
    
    UE_LOG(LogTemp, Display, TEXT("🏗️ Phase 4 Platformer Started"));
}

void ASLDeveloperBossPhase4::EndPhase()
{
    StopAutoWallAttack();
    
    if (IsValid(FallingFloor))
    {
        FallingFloor->ResetFloor();
    }
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase4::IsPhaseCompleted() const
{
    return bIsCompleted;
}

void ASLDeveloperBossPhase4::SetConfig(const FSLPhase4Config& InConfig)
{
    Config = InConfig;
    
    // Config에서 FallingFloor 설정
    if (Config.FallingFloor)
    {
        SetFallingFloor(Config.FallingFloor);
    }
}

void ASLDeveloperBossPhase4::TriggerFloorCollapse()
{
    if (!bIsPhaseActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase 4 not active! Call StartPhase() first"));
        return;
    }
    
    if (!IsValid(FallingFloor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4FallingFloor not assigned"));
        return;
    }
    
    if (Config.FloorCollapseDelay > 0.0f)
    {
        FTimerHandle Phase4DelayTimer;
        GetWorld()->GetTimerManager().SetTimer(
            Phase4DelayTimer,
            this,
            &ASLDeveloperBossPhase4::StartFloorCollapse,
            Config.FloorCollapseDelay,
            false
        );
    }
    else
    {
        StartFloorCollapse();
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

void ASLDeveloperBossPhase4::ResetFloor()
{
    if (IsValid(FallingFloor))
    {
        FallingFloor->ResetFloor();
    }
}

void ASLDeveloperBossPhase4::SetFallingFloor(ASLPhase4FallingFloor* InFallingFloor)
{
    if (IsValid(FallingFloor))
    {
        FallingFloor->OnFloorCollapseCompleted.RemoveAll(this);
    }
    
    FallingFloor = InFallingFloor;
    Config.FallingFloor = InFallingFloor;
    
    if (IsValid(FallingFloor))
    {
        FallingFloor->OnFloorCollapseCompleted.AddDynamic(this, &ASLDeveloperBossPhase4::HandleFloorCollapseCompleted);
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

void ASLDeveloperBossPhase4::OnPhaseStarted()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 4 Platformer Started"));
}

void ASLDeveloperBossPhase4::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 4 Platformer Completed"));
}

void ASLDeveloperBossPhase4::HandleFloorCollapseCompleted()
{
    if (!bIsPhaseActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase 4: Floor collapse completed - Starting auto wall attacks"));
    
    // 자동 벽 공격 시작
    StartAutoWallAttack();
    
    // 페이즈 완료 처리 (라인 시스템을 통해)
    bIsCompleted = true;
    CheckPhaseCompletion();
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

void ASLDeveloperBossPhase4::StartFloorCollapse()
{
    if (!IsValid(FallingFloor) || !bIsPhaseActive)
    {
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase 4: Starting floor collapse"));
    FallingFloor->StartFloorCollapse();
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