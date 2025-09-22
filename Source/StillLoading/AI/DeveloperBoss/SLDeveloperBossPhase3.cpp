#include "SLDeveloperBossPhase3.h"

#include "LevelSequencePlayer.h"
#include "SLDeveloperBoss.h"
#include "AI/Actors/SLMouseActor.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/SphereComponent.h"

ASLDeveloperBossPhase3::ASLDeveloperBossPhase3()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase3_Horror;
    MainMouseActor = nullptr;
    Phase3MouseActor = nullptr;
    bIsAutoWallAttackActive = false;
    CurrentWallIndex = 0;
    bIsCompleted = false;
    CurrentSequencePlayer = nullptr;
    bWaitingForCinematic = false;
}

void ASLDeveloperBossPhase3::BeginPlay()
{
    Super::BeginPlay();
}

void ASLDeveloperBossPhase3::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    StopAutoWallAttack();
    DestroyPhase3MouseActor();
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase3::StartPhase()
{
    Super::StartPhase();
    
    bIsCompleted = false;
    bWaitingForCinematic = false;
    ResetWallIndex();
    
    // 모든 벽을 표시 상태로 설정
    for (ASLLaunchableWall* Wall : AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->SetActorHiddenInGame(false);
            Wall->SetActorEnableCollision(true);
            Wall->SetActorTickEnabled(true);
        }
    }
    
    // Phase3 시작 시네마틱 재생
    PlayStartCinematic();
}

void ASLDeveloperBossPhase3::EndPhase()
{
    StopAutoWallAttack();
    DestroyPhase3MouseActor();
    
    // 메인 마우스 액터 재활성화
    if (IsValid(MainMouseActor))
    {
        MainMouseActor->SetActorHiddenInGame(false);
        MainMouseActor->SetActorEnableCollision(true);
        MainMouseActor->StartOrbiting();
    }
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase3::IsPhaseCompleted() const
{
    return bIsCompleted;
}

void ASLDeveloperBossPhase3::HandleLineDestroyed(int32 LineIndex)
{
    Super::HandleLineDestroyed(LineIndex);
    
    // 라인이 파괴될 때마다 완료 여부 확인
    if (IsValid(OwnerBoss))
    {
        bool bPhaseCompleted = OwnerBoss->IsPhaseCompleted(PhaseIndex);
        if (bPhaseCompleted)
        {
            bIsCompleted = true;
            CheckPhaseCompletion();
        }
    }
}

void ASLDeveloperBossPhase3::SetConfig(const FSLPhase3Config& InConfig)
{
    Config = InConfig;
}

void ASLDeveloperBossPhase3::StartAutoWallAttack()
{
    if (bIsAutoWallAttackActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: Auto wall attack already active"));
        return;
    }

    bIsAutoWallAttackActive = true;
    CurrentWallIndex = 0;

    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoWallAttackTimer,
            this,
            &ASLDeveloperBossPhase3::OnAutoWallAttackTimer,
            Config.InitialWallAttackDelay,
            false
        );
    }
}

void ASLDeveloperBossPhase3::StopAutoWallAttack()
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

bool ASLDeveloperBossPhase3::IsAutoWallAttackActive() const
{
    return bIsAutoWallAttackActive;
}

void ASLDeveloperBossPhase3::SetMouseActor(ASLMouseActor* InMouseActor)
{
    MainMouseActor = InMouseActor;
}

void ASLDeveloperBossPhase3::SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls)
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

void ASLDeveloperBossPhase3::OnPhaseStarted()
{
}

void ASLDeveloperBossPhase3::OnPhaseEnded()
{
}

void ASLDeveloperBossPhase3::OnAutoWallAttackTimer()
{
    if (!bIsPhaseActive || !bIsAutoWallAttackActive)
    {
        return;
    }

    ASLLaunchableWall* WallToLaunch = GetNextWall();
    
    if (IsValid(WallToLaunch))
    {
        LaunchWallWithLines(WallToLaunch);
    }

    // 다음 공격 예약
    if (bIsPhaseActive && bIsAutoWallAttackActive && IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoWallAttackTimer,
            this,
            &ASLDeveloperBossPhase3::OnAutoWallAttackTimer,
            Config.AutoWallAttackInterval,
            false
        );
    }
}

void ASLDeveloperBossPhase3::HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == Phase3MouseActor)
    {
        Phase3MouseActor = nullptr;
        
        if (bIsPhaseActive)
        {
            bIsCompleted = true;
            CheckPhaseCompletion();
        }
    }
}

void ASLDeveloperBossPhase3::SpawnPhase3MouseActor()
{
    if (IsValid(Phase3MouseActor))
    {
        DestroyPhase3MouseActor();
    }
    
    if (!Config.MouseActorClass)
    {
        return;
    }
    
    FVector SpawnLocation = MainMouseActor ? MainMouseActor->GetActorLocation() : GetActorLocation() + FVector(0, 0, 500.0f);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    Phase3MouseActor = GetWorld()->SpawnActor<ASLMouseActor>(Config.MouseActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.AddDynamic(this, &ASLDeveloperBossPhase3::HandlePhase3MouseActorDestroyed);
        Phase3MouseActor->StartPhase3HorrorMode();
        
        if (UBattleComponent* MouseBattleComp = Phase3MouseActor->FindComponentByClass<UBattleComponent>())
        {
            MouseBattleComp->SetComponentTickEnabled(false);
        }
        
        if (USphereComponent* MouseCollision = Phase3MouseActor->FindComponentByClass<USphereComponent>())
        {
            MouseCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            MouseCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
        }
    }
}

void ASLDeveloperBossPhase3::DestroyPhase3MouseActor()
{
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.RemoveAll(this);
        Phase3MouseActor->Destroy();
        Phase3MouseActor = nullptr;
    }
}

ASLLaunchableWall* ASLDeveloperBossPhase3::GetNextWall()
{
    if (AvailableWalls.Num() == 0)
    {
        return nullptr;
    }

    ASLLaunchableWall* SelectedWall = nullptr;

    if (Config.bRandomWallSelection)
    {
        TArray<ASLLaunchableWall*> LaunchableWalls;
        
        for (ASLLaunchableWall* Wall : AvailableWalls)
        {
            if (IsValid(Wall) && Wall->CanLaunch())
            {
                LaunchableWalls.Add(Wall);
            }
        }
        
        if (LaunchableWalls.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, LaunchableWalls.Num() - 1);
            SelectedWall = LaunchableWalls[RandomIndex];
        }
    }
    else
    {
        int32 WallCount = AvailableWalls.Num();
        int32 CheckedWalls = 0;
        
        while (CheckedWalls < WallCount)
        {
            if (AvailableWalls.IsValidIndex(CurrentWallIndex))
            {
                ASLLaunchableWall* Wall = AvailableWalls[CurrentWallIndex];
                
                if (IsValid(Wall) && Wall->CanLaunch())
                {
                    SelectedWall = Wall;
                    CurrentWallIndex = (CurrentWallIndex + 1) % WallCount;
                    break;
                }
            }
            
            CurrentWallIndex = (CurrentWallIndex + 1) % WallCount;
            CheckedWalls++;
        }
    }

    return SelectedWall;
}

void ASLDeveloperBossPhase3::ResetWallIndex()
{
    CurrentWallIndex = 0;
}

void ASLDeveloperBossPhase3::LaunchWallWithLines(ASLLaunchableWall* Wall)
{
    if (!IsValid(Wall) || !IsValid(OwnerBoss))
    {
        return;
    }
    
    // 벽 발사
    Wall->LaunchWallToPlayer();
    
    // 즉시 라인 활성화
    OwnerBoss->ActivateConnectedLines(PhaseIndex, Wall);
}

void ASLDeveloperBossPhase3::PlayStartCinematic()
{
    
    int32 CinematicIndex = 0; // 시작 시네마틱
    
    if (!Config.Cinematics.IsValidIndex(CinematicIndex))
    {
        StartPhaseAfterCinematic(); // 시네마틱 없으면 바로 게임플레이 시작
        return;
    }
    
    if (!IsValid(Config.Cinematics[CinematicIndex]))
    {
        StartPhaseAfterCinematic();
        return;
    }
    
    
    bWaitingForCinematic = true;
    
    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    PlaybackSettings.bHideHud = false;
    PlaybackSettings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;
    PlaybackSettings.bDisableLookAtInput = false;
    PlaybackSettings.bDisableMovementInput = false;
    
    ALevelSequenceActor* SequenceActor = nullptr;
    CurrentSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        GetWorld(),
        Config.Cinematics[CinematicIndex],
        PlaybackSettings,
        SequenceActor
    );
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase3::OnCinematicFinished);
        CurrentSequencePlayer->Play();
    }
    else
    {
        StartPhaseAfterCinematic();
    }
}

void ASLDeveloperBossPhase3::OnCinematicFinished()
{
    
    bWaitingForCinematic = false;
    
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    StartPhaseAfterCinematic();
}

void ASLDeveloperBossPhase3::StartPhaseAfterCinematic()
{
    
    // 메인 마우스 액터 비활성화
    if (IsValid(MainMouseActor))
    {
        MainMouseActor->StopOrbiting();
        MainMouseActor->SetActorHiddenInGame(true);
        MainMouseActor->SetActorEnableCollision(false);
    }
    
    // Phase3 전용 마우스 액터 생성
    SpawnPhase3MouseActor();
    
    // 자동 벽 공격 시작
    StartAutoWallAttack();
}