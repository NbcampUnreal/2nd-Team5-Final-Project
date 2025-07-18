#include "SLDeveloperBossPhase5.h"
#include "SLDeveloperBoss.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "AI/Actors/SLMouseActor.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASLDeveloperBossPhase5::ASLDeveloperBossPhase5()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase5_Final;
    MouseActor = nullptr;
    bIsCompleted = false;
}

void ASLDeveloperBossPhase5::BeginPlay()
{
    Super::BeginPlay();
    
    // Config에서 사용 가능한 벽들 설정
    SetAvailableWalls(Config.AvailableWalls);
}

void ASLDeveloperBossPhase5::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(GetWorld()) && WallAttackTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(WallAttackTimer);
    }
    
    // 모든 벽의 자동 리셋 비활성화
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->SetAutoResetEnabled(false);
            Wall->OnAllWallPartsLaunched.RemoveAll(this);
            Wall->OnWallHitMouseActor.RemoveAll(this);
        }
    }
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase5::StartPhase()
{
    Super::StartPhase();
    
    if (!IsValid(MouseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase5: No mouse actor assigned"));
        bIsCompleted = true;
        CheckPhaseCompletion();
        return;
    }

    bIsCompleted = false;
    ActiveWalls.Empty();

    InitializeWallAttack();
    
    // Phase 5에서만 벽 자동 리셋 활성화
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->SetAutoResetEnabled(true);
        }
    }

    // 마우스 액터 공격 불가능 설정
    if (IsValid(MouseActor))
    {
        MouseActor->DisableAttackability();
        MouseActor->SetActorHiddenInGame(false);
        MouseActor->SetActorEnableCollision(true);
        MouseActor->StartOrbiting();
    }

    // 벽 공격 타이머 시작
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            WallAttackTimer, 
            this,
            &ASLDeveloperBossPhase5::LaunchNextWall,
            Config.WallAttackInterval, 
            true,                     
            Config.WallAttackDelay     
        );
    }
}

void ASLDeveloperBossPhase5::EndPhase()
{
    if (IsValid(GetWorld()) && WallAttackTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(WallAttackTimer);
    }
    
    ActiveWalls.Empty();

    // 모든 벽의 자동 리셋 비활성화
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->SetAutoResetEnabled(false);
            Wall->OnAllWallPartsLaunched.RemoveAll(this);
            Wall->OnWallHitMouseActor.RemoveAll(this);
        }
    }
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase5::IsPhaseCompleted() const
{
    return bIsCompleted;
}

void ASLDeveloperBossPhase5::LaunchNextWall()
{
    if (!bIsPhaseActive)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5 wall attack stopped - Phase5 not active"));
        if (IsValid(GetWorld()) && WallAttackTimer.IsValid()) 
        {
            GetWorld()->GetTimerManager().ClearTimer(WallAttackTimer);
        }
        return;
    }

    if (Config.bEnableMultiWallAttack)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Launching Multi Wall Attack."));
        LaunchMultiWallAttack();
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Launching Single Wall Attack."));
        TArray<ASLLaunchableWall*> LaunchableWalls;
        for (ASLLaunchableWall* Wall : AvailableWalls)
        {
            if (IsValid(Wall) && Wall->CanLaunch())
            {
                LaunchableWalls.Add(Wall);
            }
        }

        if (LaunchableWalls.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Phase5 (Single Mode): No available walls to launch. Waiting for next interval."));
            return;
        }

        int32 RandomIndex = FMath::RandRange(0, LaunchableWalls.Num() - 1);
        ASLLaunchableWall* SelectedWall = LaunchableWalls[RandomIndex];
        LaunchSingleWall(SelectedWall);
    }
}

void ASLDeveloperBossPhase5::LaunchMultiWallAttack()
{
    // 활성화된 벽 정리 (무효한 참조 제거)
    CleanupInactiveWalls();
    
    // 활성 벽 개수 제한 확인
    if (Config.bLimitActiveWalls && ActiveWalls.Num() >= Config.MaxActiveWalls)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Cannot launch - too many active walls (%d/%d)"), 
               ActiveWalls.Num(), Config.MaxActiveWalls);
        return;
    }
    
    TArray<ASLLaunchableWall*> LaunchableWalls;
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall) && Wall->CanLaunch() && !ActiveWalls.Contains(Wall))
        {
            LaunchableWalls.Add(Wall);
        }
    }

    if (LaunchableWalls.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase5: No available walls for launch"));
        return;
    }

    // 발사 가능한 벽 개수 계산
    int32 MaxLaunchable = Config.bLimitActiveWalls ? 
        FMath::Min(Config.MaxActiveWalls - ActiveWalls.Num(), Config.MaxSimultaneousWalls) : 
        Config.MaxSimultaneousWalls;
    
    int32 WallsToLaunchCount = FMath::Min(MaxLaunchable, LaunchableWalls.Num());
    
    UE_LOG(LogTemp, Display, TEXT("Phase5: Launching %d walls (Active: %d/%d)"), 
           WallsToLaunchCount, ActiveWalls.Num(), Config.MaxActiveWalls);

    for (int32 i = 0; i < WallsToLaunchCount; i++)
    {
        if (LaunchableWalls.Num() == 0) break;

        int32 RandomIndex = FMath::RandRange(0, LaunchableWalls.Num() - 1);
        ASLLaunchableWall* SelectedWall = LaunchableWalls[RandomIndex];
        LaunchableWalls.RemoveAt(RandomIndex);

        float RandomDelay = FMath::FRandRange(Config.MultiWallDelayMin, Config.MultiWallDelayMax);

        FTimerHandle WallTimer;
        GetWorld()->GetTimerManager().SetTimer(
            WallTimer,
            [this, SelectedWall]() { 
                if (IsValid(SelectedWall)) 
                {
                    LaunchSingleWall(SelectedWall);
                }
            },
            RandomDelay,
            false
        );
    }
}

void ASLDeveloperBossPhase5::SetMouseActor(ASLMouseActor* InMouseActor)
{
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    
    MouseActor = InMouseActor;
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.AddDynamic(this, &ASLDeveloperBossPhase5::HandleMouseActorDestroyed);
    }
}

void ASLDeveloperBossPhase5::SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls)
{
    // 기존 벽들의 이벤트 정리
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->OnAllWallPartsLaunched.RemoveAll(this);
            Wall->OnWallHitMouseActor.RemoveAll(this);
        }
    }
    
    AvailableWalls.Empty();
    for (ASLLaunchableWall* Wall : InWalls)
    {
        if (IsValid(Wall))
        {
            AvailableWalls.Add(Wall);
        }
    }
    
    Config.AvailableWalls = AvailableWalls;
}

void ASLDeveloperBossPhase5::OnPhaseStarted()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 5 Final Started"));
}

void ASLDeveloperBossPhase5::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 5 Final Completed"));
}

void ASLDeveloperBossPhase5::SetConfig(const FSLPhase5Config& InConfig)
{
    Config = InConfig;
    
    // Config에서 사용 가능한 벽들 설정
    SetAvailableWalls(Config.AvailableWalls);

}

void ASLDeveloperBossPhase5::OnWallCompleted(ASLLaunchableWall* CompletedWall)
{
    if (!bIsPhaseActive)
    {
        return;
    }

    ActiveWalls.Remove(CompletedWall);
    
    if (!Config.bEnableMultiWallAttack)
    {
        LaunchReplacementWall();
    }
}

void ASLDeveloperBossPhase5::OnWallHitMouseActor(ASLMouseActor* HitMouseActor, int32 WallPartIndex)
{
    if (!IsValid(HitMouseActor) || !bIsPhaseActive)
    {
        return;
    }
    
    HitMouseActor->ApplyWallStun(5.0f);
    OnPhase5MouseActorHit.Broadcast(HitMouseActor, WallPartIndex);
}

void ASLDeveloperBossPhase5::HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == MouseActor)
    {
        MouseActor = nullptr;
        
        if (bIsPhaseActive) 
        {
            bIsCompleted = true;
            
            ActiveWalls.Empty();

            // 모든 벽의 자동 리셋 비활성화
            for (ASLLaunchableWall* Wall : AvailableWalls)
            {
                if (IsValid(Wall))
                {
                    Wall->SetAutoResetEnabled(false);
                }
            }
            
            if (IsValid(GetWorld()) && WallAttackTimer.IsValid())
            {
                GetWorld()->GetTimerManager().ClearTimer(WallAttackTimer);
                UE_LOG(LogTemp, Display, TEXT("Phase5 wall attack timer cleared - Main Mouse Actor destroyed for Phase 5"));
            }
            
            CheckPhaseCompletion();
        }
    }
}

void ASLDeveloperBossPhase5::LaunchSingleWall(ASLLaunchableWall* Wall)
{
    if (!IsValid(Wall) || !bIsPhaseActive || !Wall->CanLaunch()) 
    {
        return;
    }
    
    // 활성 벽 개수 제한 확인
    if (ActiveWalls.Num() >= Config.MaxActiveWalls)
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Phase5: Launching wall %s"), *Wall->GetName());

    // 활성 벽 목록에 추가
    ActiveWalls.AddUnique(Wall);
    
    Wall->OnAllWallPartsLaunched.AddUniqueDynamic(this, &ASLDeveloperBossPhase5::OnWallCompleted);
    Wall->OnWallHitMouseActor.AddUniqueDynamic(this, &ASLDeveloperBossPhase5::OnWallHitMouseActor);

    Wall->LaunchWallToPlayer();
}

void ASLDeveloperBossPhase5::LaunchReplacementWall()
{
    if (!bIsPhaseActive)
    {
        return;
    }

    TArray<ASLLaunchableWall*> LaunchableWalls;
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall) && Wall->CanLaunch())
        {
            LaunchableWalls.Add(Wall);
        }
    }

    if (LaunchableWalls.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase5: No replacement walls available"));
        return;
    }

    int32 RandomIndex = FMath::RandRange(0, LaunchableWalls.Num() - 1);
    ASLLaunchableWall* ReplacementWall = LaunchableWalls[RandomIndex];

    float RandomDelay = FMath::FRandRange(Config.MultiWallDelayMin, Config.MultiWallDelayMax);
    
    FTimerHandle ReplacementTimer;
    GetWorld()->GetTimerManager().SetTimer(
        ReplacementTimer,
        [this, ReplacementWall]()
        {
            if (IsValid(ReplacementWall)) 
            {
                LaunchSingleWall(ReplacementWall);
            }
        },
        RandomDelay,
        false
    );
}

void ASLDeveloperBossPhase5::CleanupInactiveWalls()
{
    ActiveWalls.RemoveAll([](ASLLaunchableWall* Wall) {
        return !IsValid(Wall) || Wall->CanLaunch(); // 리셋되어 다시 발사 가능한 상태면 비활성으로 간주
    });
}

void ASLDeveloperBossPhase5::InitializeWallAttack()
{
    AvailableWalls.Empty();

    // Config에서 사용 가능한 벽들을 가져오거나, 필요시 모든 페이즈의 벽들을 수집
    if (Config.AvailableWalls.Num() > 0)
    {
        for (ASLLaunchableWall* Wall : Config.AvailableWalls)
        {
            if (IsValid(Wall))
            {
                AvailableWalls.Add(Wall);
            }
        }
    }
    
    ResetAllWalls();
}

void ASLDeveloperBossPhase5::ResetAllWalls()
{
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->ResetWall();
        }
    }
}