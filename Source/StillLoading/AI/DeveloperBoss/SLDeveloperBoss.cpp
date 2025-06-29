#include "SLDeveloperBoss.h"

#include "LevelSequencePlayer.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/SLAIBaseCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AI/Actors/SLDeveloperRoomCable.h"
#include "AI/Actors/SLDeveloperRoomSpace.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "AI/Actors/SLMouseActor.h"
#include "AI/Actors/SLPhase4FallingFloor.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/CapsuleComponent.h"

ASLDeveloperBoss::ASLDeveloperBoss()
{
    PrimaryActorTick.bCanEverTick = false;

    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    bIsFightStarted = false;
    bCanLaunchWall = true;
    
    WallAttackCooldown = 3.0f;
    CurrentWall = nullptr;
    CurrentPhase5Wall = nullptr;

    Phase1BossHealthMultiplier = 0.3f;
    Phase1BossSpawnDelay = 2.0f;
    Phase1BossSpawnOffset = FVector(300.0f, 0.0f, 0.0f);
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;

    Phase2Room = nullptr;
    bIsPhase2Active = false;

    Phase3WallAttackDelay = 5.0f;
    Phase3WallIndex = -1;
    Phase3WallAttackInterval = 3.0f;
    bIsPhase3Active = false;
    bIsPhase3WallAttackScheduled = false;
    Phase3UsedWallIndex = 0;
    Phase3MouseActor = nullptr;
    Phase3AutoWallAttackInterval = 4.0f;
    Phase3InitialWallAttackDelay = 2.0f;
    bPhase3RandomWallSelection = false;
    bIsPhase3AutoWallAttackActive = false;
    Phase3CurrentWallIndex = 0;

    Phase5MaxSimultaneousWalls = 3;
    Phase5MultiWallDelayMin = 0.2f;
    Phase5MultiWallDelayMax = 1.0f;
    bPhase5EnableMultiWallAttack = true;
    Phase5WallAttackInterval = 2.0f;
    Phase5WallAttackDelay = 1.0f;
    Phase5WallResetDelay = 1.5f;
    bIsPhase5Active = false;
    Phase5MaxActiveWalls = 2;
    bPhase5LimitActiveWalls = true;
    
    MouseActor = nullptr;
    MouseActorClass = nullptr;

    PendingLineActivation.PhaseIndex = -1;
    PendingLineActivation.LaunchedWall = nullptr;

    Phase4FallingFloor = nullptr;
    Phase4FloorCollapseDelay = 1.0f;
    bIsPhase4Active = false;
}

void ASLDeveloperBoss::BeginPlay()
{
    Super::BeginPlay();
    SetupPhaseLines();

    if (IsValid(Phase4FallingFloor))
    {
        Phase4FallingFloor->OnFloorCollapseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhase4FloorCollapseCompleted);
    }
}

void ASLDeveloperBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    bIsPhase1Active = false;
    bIsPhase3Active = false;
    bIsPhase5Active = false;
    bIsPhase4Active = false;
    bIsPhase3WallAttackScheduled = false;
    bIsPhase3AutoWallAttackActive = false;
    Phase5ActiveWalls.Empty();
    
    if (GetWorld())
    {
        if (Phase3WallAttackTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase3WallAttackTimer);
        }
        if (Phase5WallAttackTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase5WallAttackTimer);
        }
        if (Phase3AutoWallAttackTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase3AutoWallAttackTimer);
        }
        if (Phase4AutoWallAttackTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase4AutoWallAttackTimer);
        }
    }
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    if (IsValid(CurrentPhase5Wall))
    {
        CurrentPhase5Wall->OnAllWallPartsLaunched.RemoveAll(this);
        CurrentPhase5Wall = nullptr;
    }
    if (IsValid(Phase4FallingFloor))
    {
        Phase4FallingFloor->OnFloorCollapseCompleted.RemoveAll(this);
    }
    ResetCurrentWall();
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
}

void ASLDeveloperBoss::InitializeBossFight()
{
    if (bIsFightStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss fight already initialized"));
        return;
    }

    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    PhaseDestroyedLinesCount.Empty();
    bCanLaunchWall = true;
    
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;
    
    for (auto& PhaseData : PhaseLineDataMap)
    {
        for (ASLDeveloperRoomCable* Line : PhaseData.Value.Lines)
        {
            if (IsValid(Line))
            {
                Line->DeactivateLine();
            }
        }
    }
}

void ASLDeveloperBoss::TriggerFirstWallDuringDialogue()
{
    if (bIsFightStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss fight already started"));
        return;
    }

    bIsFightStarted = true;
    ManualLaunchWallAttack();
}

void ASLDeveloperBoss::ManualLaunchWallAttack(int32 PhaseIndex, int32 WallIndex)
{
    if (!IsPlayerAlive())
    {
        return;
    }
    
    if (!CanLaunchWallAttack(PhaseIndex))
    {
        return;
    }

    int32 TargetPhaseIndex = (PhaseIndex >= 0) ? PhaseIndex : static_cast<int32>(CurrentPhase);
    
    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(TargetPhaseIndex);
    if (!PhaseData)
    {
        return;
    }
    
    ASLLaunchableWall* TargetWall = nullptr;

    if (WallIndex >= 0 && PhaseData->WallConnections.IsValidIndex(WallIndex))
    {
        TargetWall = PhaseData->WallConnections[WallIndex].Wall;
    }
    else
    {
        for (int32 i = 0; i < PhaseData->WallConnections.Num(); i++)
        {
            const FWallLineConnection& Connection = PhaseData->WallConnections[i];
            if (IsValid(Connection.Wall) && Connection.Wall->CanLaunch())
            {
                TargetWall = Connection.Wall;
                break;
            }
        }
    }

    if (IsValid(TargetWall) && TargetWall->CanLaunch())
    {
        LaunchSpecificWall(TargetWall);
        PendingLineActivation.PhaseIndex = TargetPhaseIndex;
        PendingLineActivation.LaunchedWall = TargetWall;
    }
}

void ASLDeveloperBoss::ManualActivateLine(int32 PhaseIndex, int32 LineIndex)
{
    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(PhaseIndex);
    if (!PhaseData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase %d not found"), PhaseIndex);
        return;
    }

    if (!PhaseData->Lines.IsValidIndex(LineIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Line index %d not valid for phase %d"), LineIndex, PhaseIndex);
        return;
    }

    ASLDeveloperRoomCable* TargetLine = PhaseData->Lines[LineIndex];
    if (IsValid(TargetLine))
    {
        TargetLine->ActivateLine();
    }
}

void ASLDeveloperBoss::ActivateConnectedLines(int32 PhaseIndex, ASLLaunchableWall* LaunchedWall)
{
    if (!IsValid(LaunchedWall))
    {
        UE_LOG(LogTemp, Warning, TEXT("ActivateConnectedLines: LaunchedWall is invalid"));
        return;
    }

    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(PhaseIndex);
    if (!PhaseData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase %d not found for line activation"), PhaseIndex);
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("🔍 ActivateConnectedLines: Phase %d, Wall %s"), 
           PhaseIndex, *LaunchedWall->GetName());
    UE_LOG(LogTemp, Display, TEXT("   Available wall connections: %d"), PhaseData->WallConnections.Num());

    bool bFoundConnection = false;
    for (int32 i = 0; i < PhaseData->WallConnections.Num(); i++)
    {
        const FWallLineConnection& Connection = PhaseData->WallConnections[i];
        UE_LOG(LogTemp, Display, TEXT("   Checking connection [%d]: Wall %s"), 
               i, Connection.Wall ? *Connection.Wall->GetName() : TEXT("NULL"));
        
        if (Connection.Wall == LaunchedWall)
        {
            bFoundConnection = true;
            UE_LOG(LogTemp, Display, TEXT("   ✅ Found matching connection! Activating %d lines"), 
                   Connection.ConnectedLineIndices.Num());
            
            for (int32 LineIndex : Connection.ConnectedLineIndices)
            {
                if (PhaseData->Lines.IsValidIndex(LineIndex))
                {
                    ASLDeveloperRoomCable* LineToActivate = PhaseData->Lines[LineIndex];
                    if (IsValid(LineToActivate))
                    {
                        LineToActivate->ActivateLine();
                        UE_LOG(LogTemp, Display, TEXT("   📍 Activated line %d in phase %d"), LineIndex, PhaseIndex);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("   ❌ Line %d is invalid"), LineIndex);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("   ❌ Line index %d out of range (max: %d)"), 
                           LineIndex, PhaseData->Lines.Num() - 1);
                }
            }
            return;
        }
    }
    
    if (!bFoundConnection)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ No connection found for wall %s in phase %d"), 
               *LaunchedWall->GetName(), PhaseIndex);
    }
}

EDeveloperBossPhase ASLDeveloperBoss::GetCurrentPhase() const
{
    return CurrentPhase;
}

int32 ASLDeveloperBoss::GetCurrentPhaseIndex() const
{
    return static_cast<int32>(CurrentPhase);
}

int32 ASLDeveloperBoss::GetPhaseDestroyedLinesCount(int32 PhaseIndex) const
{
    if (const int32* Count = PhaseDestroyedLinesCount.Find(PhaseIndex))
    {
        return *Count;
    }
    return 0;
}

int32 ASLDeveloperBoss::GetCurrentPhaseDestroyedLinesCount() const
{
    return GetPhaseDestroyedLinesCount(static_cast<int32>(CurrentPhase));
}

bool ASLDeveloperBoss::CanLaunchWallAttack(int32 PhaseIndex) const
{
    if (!bCanLaunchWall)
    {
        return false;
    }

    int32 TargetPhaseIndex = (PhaseIndex >= 0) ? PhaseIndex : static_cast<int32>(CurrentPhase);
    
    const FPhaseLineData* PhaseData = PhaseLineDataMap.Find(TargetPhaseIndex);
    if (!PhaseData)
    {
        return false;
    }

    return PhaseData->WallConnections.Num() > 0;
}

bool ASLDeveloperBoss::IsPhaseCompleted(int32 PhaseIndex) const
{
    const FPhaseLineData* PhaseData = PhaseLineDataMap.Find(PhaseIndex);
    if (!PhaseData)
    {
        return false;
    }

    int32 DestroyedCount = GetPhaseDestroyedLinesCount(PhaseIndex);
    return DestroyedCount >= PhaseData->Lines.Num();
}

bool ASLDeveloperBoss::IsCurrentPhaseCompleted() const
{
    return IsPhaseCompleted(static_cast<int32>(CurrentPhase));
}

bool ASLDeveloperBoss::IsPhase1Active() const
{
    return bIsPhase1Active;
}

int32 ASLDeveloperBoss::GetPhase1BossesRemaining() const
{
    if (!bIsPhase1Active)
    {
        return 0;
    }
    
    int32 BossesToSpawn = AvailableBossClasses.Num() - Phase1CurrentBossIndex;
    int32 SpawnedBossesAlive = SpawnedBosses.Num();
    
    return BossesToSpawn + SpawnedBossesAlive;
}

void ASLDeveloperBoss::SpawnMouseActor()
{
    if (IsValid(MouseActor))
    {
        DestroyMouseActor();
    }
    
    if (!MouseActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mouse Actor class not specified"));
        return;
    }
    
    FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 500.0f);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    MouseActor = GetWorld()->SpawnActor<ASLMouseActor>(MouseActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandleMouseActorDestroyed);
        MouseActor->SetActorHiddenInGame(false);
        MouseActor->SetActorEnableCollision(true);
        MouseActor->StartOrbiting();
    }
}

void ASLDeveloperBoss::DestroyMouseActor()
{
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
        MouseActor->Destroy();
        MouseActor = nullptr;
    }
}

void ASLDeveloperBoss::ActivateMouseActor()
{
    if (!IsValid(MouseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("No Mouse Actor to activate"));
        return;
    }
    
    MouseActor->SetActorHiddenInGame(false);
    MouseActor->SetActorEnableCollision(true);
    MouseActor->StartOrbiting();
}

void ASLDeveloperBoss::DeactivateMouseActor()
{
    if (!IsValid(MouseActor))
    {
        return;
    }
    
    MouseActor->StopOrbiting();
    MouseActor->SetActorHiddenInGame(true);
    MouseActor->SetActorEnableCollision(false);
}

ASLMouseActor* ASLDeveloperBoss::GetMouseActor() const
{
    return MouseActor;
}

void ASLDeveloperBoss::StartPhase3AutoWallAttack()
{
    if (bIsPhase3AutoWallAttackActive || !bIsPhase3Active)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3 Auto Wall Attack already active or Phase3 not active"));
        return;
    }

    bIsPhase3AutoWallAttackActive = true;
    Phase3CurrentWallIndex = 0;

    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            Phase3AutoWallAttackTimer,
            this,
            &ASLDeveloperBoss::OnPhase3AutoWallAttackTimer,
            Phase3InitialWallAttackDelay,
            false
        );
    }
}

void ASLDeveloperBoss::StopPhase3AutoWallAttack()
{
    if (!bIsPhase3AutoWallAttackActive)
    {
        return;
    }

    bIsPhase3AutoWallAttackActive = false;

    if (IsValid(GetWorld()) && Phase3AutoWallAttackTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(Phase3AutoWallAttackTimer);
    }

}

bool ASLDeveloperBoss::IsPhase3AutoWallAttackActive() const
{
    return bIsPhase3AutoWallAttackActive;
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
}

void ASLDeveloperBoss::TestDestroyNextLine()
{
    int32 CurrentPhaseIndex = static_cast<int32>(CurrentPhase);
    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(CurrentPhaseIndex);
    
    if (!PhaseData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Current phase data not found"));
        return;
    }

    int32 DestroyedCount = GetCurrentPhaseDestroyedLinesCount();
    
    if (DestroyedCount >= PhaseData->Lines.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("All lines in current phase already destroyed"));
        return;
    }

    if (PhaseData->Lines.IsValidIndex(DestroyedCount) && IsValid(PhaseData->Lines[DestroyedCount]))
    {
        PhaseData->Lines[DestroyedCount]->DestroyLine();
    }
}

void ASLDeveloperBoss::TestCompleteCurrentPhase()
{
    int32 CurrentPhaseIndex = static_cast<int32>(CurrentPhase);
    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(CurrentPhaseIndex);
    
    if (PhaseData)
    {
        for (ASLDeveloperRoomCable* Line : PhaseData->Lines)
        {
            if (IsValid(Line) && Line->GetCurrentLineState() != EBossLineState::Destroyed)
            {
                Line->DestroyLine();
            }
        }
    }
}

void ASLDeveloperBoss::DebugPhaseData(int32 PhaseIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("=== DEBUG PHASE %d ==="), PhaseIndex);
    
    FPhaseLineData* PhaseData = PhaseLineDataMap.Find(PhaseIndex);
    if (!PhaseData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Phase %d NOT FOUND in PhaseLineDataMap!"), PhaseIndex);
        UE_LOG(LogTemp, Warning, TEXT("Available phases:"));
        for (auto& Data : PhaseLineDataMap)
        {
            UE_LOG(LogTemp, Warning, TEXT("  - Phase %d"), Data.Key);
        }
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("✅ Phase %d found"), PhaseIndex);
    UE_LOG(LogTemp, Display, TEXT("Lines count: %d"), PhaseData->Lines.Num());
    UE_LOG(LogTemp, Display, TEXT("Wall connections count: %d"), PhaseData->WallConnections.Num());
    
    for (int32 i = 0; i < PhaseData->WallConnections.Num(); i++)
    {
        const FWallLineConnection& Connection = PhaseData->WallConnections[i];
        if (IsValid(Connection.Wall))
        {
            bool bCanLaunch = Connection.Wall->CanLaunch();
            UE_LOG(LogTemp, Display, TEXT("  Wall [%d]: %s - CanLaunch: %s"), 
                   i, *Connection.Wall->GetName(), bCanLaunch ? TEXT("YES") : TEXT("NO"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("  Wall [%d]: ❌ NULL"), i);
        }
    }
}

void ASLDeveloperBoss::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (DeadBoss)
    {
        OnBossCharacterDeath.Broadcast(DeadBoss);
        UnregisterBossEvents(DeadBoss);
        SpawnedBosses.Remove(DeadBoss);

        if (IsValid(DeadBoss))
        {
            DeadBoss->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            DeadBoss->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            DeadBoss->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
            DeadBoss->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
        }

        if (bIsPhase1Active)
        {
            HandlePhase1BossDeath(DeadBoss);
        }
        
        if (SpawnedBosses.Num() == 0 && !bIsPhase1Active && CanLaunchWallAttack())
        {
            ManualLaunchWallAttack();
        }
    }
}

void ASLDeveloperBoss::HandlePatternFinished(ASLAIBaseCharacter* Boss)
{
    if (Boss)
    {
        OnBossPatternFinished.Broadcast(Boss);
    }
}

void ASLDeveloperBoss::HandleLineDestroyed(int32 LineIndex)
{
    int32 PhaseIndex = -1;
    int32 PhaseLineIndex = -1;
    
    for (auto& PhaseData : PhaseLineDataMap)
    {
        for (int32 i = 0; i < PhaseData.Value.Lines.Num(); i++)
        {
            if (IsValid(PhaseData.Value.Lines[i]) && PhaseData.Value.Lines[i]->GetLineIndex() == LineIndex)
            {
                PhaseIndex = PhaseData.Key;
                PhaseLineIndex = i;
                break;
            }
        }
        if (PhaseIndex >= 0) break;
    }

    if (PhaseIndex < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not find phase for destroyed line %d"), LineIndex);
        return;
    }

    int32& DestroyedCount = PhaseDestroyedLinesCount.FindOrAdd(PhaseIndex);
    DestroyedCount++;

    OnBossLineDestroyed.Broadcast(PhaseIndex, PhaseLineIndex);
    
    if (PhaseIndex == 1 && PhaseLineIndex == 1 && IsValid(MouseActor))
    {
        MouseActor->StopOrbiting();
    }

    if (PhaseIndex == static_cast<int32>(EDeveloperBossPhase::Phase3_Horror) && bIsPhase3Active)
    {
        CheckPhase3Completion();
    }

    CheckPhaseCompletion(PhaseIndex);
}

void ASLDeveloperBoss::HandleWallAttackFinished(ASLLaunchableWall* LaunchedWall)
{
    UE_LOG(LogTemp, Display, TEXT("🏁 HandleWallAttackFinished called"));
    
    if (PendingLineActivation.LaunchedWall.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("   Processing pending line activation for Phase %d"), 
               PendingLineActivation.PhaseIndex);
        
        ActivateConnectedLines(PendingLineActivation.PhaseIndex, PendingLineActivation.LaunchedWall.Get());
        
        PendingLineActivation.PhaseIndex = -1;
        PendingLineActivation.LaunchedWall = nullptr;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("   ❌ No pending line activation found"));
    }
    
    ResetCurrentWall();
}

void ASLDeveloperBoss::HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == MouseActor)
    {
        MouseActor = nullptr;
        
        if (bIsPhase5Active) 
        {
            bIsPhase5Active = false; 

            Phase5ActiveWalls.Empty();

            for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
            {
                if (IsValid(Wall))
                {
                    Wall->SetAutoResetEnabled(false);
                }
            }
            
            if (GetWorld() && Phase5WallAttackTimer.IsValid())
            {
                GetWorld()->GetTimerManager().ClearTimer(Phase5WallAttackTimer);
                UE_LOG(LogTemp, Display, TEXT("Phase5 wall attack timer cleared - Main Mouse Actor destroyed for Phase 5"));
            }
            
            OnPhase5FinalCompleted.Broadcast();
            OnDeveloperBossPatternFinished.Broadcast(); 
        }
    }
}

void ASLDeveloperBoss::HandlePhase2RoomEscape(ASLDeveloperRoomSpace* Room)
{
    if (!bIsPhase2Active || Room != Phase2Room)
    {
        return;
    }
    
    bIsPhase2Active = false;
    
    if (IsValid(Phase2Room))
    {
        Phase2Room->OnRoomEscapeWallDestroyed.RemoveAll(this);
        Phase2Room->CleanupNPCs();
        Phase2Room->DeactivateRoom();
    }

    if (IsValid(MouseActor))
    {
        ActivateMouseActor();
    }
    
    OnPhase2HackSlashCompleted.Broadcast();
}

void ASLDeveloperBoss::OnWallCooldownFinishedInternal()
{
    bCanLaunchWall = true;
    OnWallCooldownFinished.Broadcast();
}

void ASLDeveloperBoss::HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == Phase3MouseActor)
    {
        Phase3MouseActor = nullptr;
        
        if (bIsPhase3Active)
        {
            bIsPhase3Active = false;
            bIsPhase3WallAttackScheduled = false;
            
            StopPhase3AutoWallAttack();
            
            if (GetWorld() && Phase3WallAttackTimer.IsValid())
            {
                GetWorld()->GetTimerManager().ClearTimer(Phase3WallAttackTimer);
            }
            
            ActivateMouseActor();
            OnPhase3HorrorCompleted.Broadcast();
        }
    }
}

void ASLDeveloperBoss::ExecutePhase3WallAttack()
{
    if (!bIsPhase3Active || !bIsPhase3WallAttackScheduled)
    {
        return;
    }
    
    bIsPhase3WallAttackScheduled = false;
    
    int32 Phase3Index = static_cast<int32>(EDeveloperBossPhase::Phase3_Horror);
    
    FPhaseLineData* Phase3Data = PhaseLineDataMap.Find(Phase3Index);
    if (!Phase3Data || Phase3Data->WallConnections.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: No wall connections found"));
        return;
    }
    
    int32 WallIndexToUse = -1;
    
    if (Phase3WallIndex >= 0)
    {
        WallIndexToUse = Phase3WallIndex;
    }
    else
    {
        if (Phase3UsedWallIndex < Phase3Data->WallConnections.Num())
        {
            WallIndexToUse = Phase3UsedWallIndex;
            Phase3UsedWallIndex++;
        }
        else
        {
            Phase3UsedWallIndex = 0;
            WallIndexToUse = 0;
        }
    }
    
    ManualLaunchWallAttack(Phase3Index, WallIndexToUse);
    
    UE_LOG(LogTemp, Display, TEXT("Phase3: Wall attack executed (Wall Index: %d)"), WallIndexToUse);
}

void ASLDeveloperBoss::CheckPhase3Completion()
{
    if (!bIsPhase3Active)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3 not active, skipping completion check"));
        return;
    }
    
    int32 Phase3Index = static_cast<int32>(EDeveloperBossPhase::Phase3_Horror);
    
    bool bPhaseCompleted = IsPhaseCompleted(Phase3Index);
    
    int32 DestroyedCount = GetPhaseDestroyedLinesCount(Phase3Index);
    FPhaseLineData* Phase3Data = PhaseLineDataMap.Find(Phase3Index);
    int32 TotalLines = Phase3Data ? Phase3Data->Lines.Num() : 0;
    
    /*UE_LOG(LogTemp, Warning, TEXT("Destroyed Lines: %d / %d"), DestroyedCount, TotalLines);
    UE_LOG(LogTemp, Warning, TEXT("Phase Completed: %s"), bPhaseCompleted ? TEXT("YES") : TEXT("NO"));*/
    
    if (bPhaseCompleted)
    {
        
        // Phase3 정리
        bIsPhase3Active = false;
        bIsPhase3WallAttackScheduled = false;
        StopPhase3AutoWallAttack();
        
        if (GetWorld() && Phase3WallAttackTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase3WallAttackTimer);
        }
        
        DestroyPhase3MouseActor();
        ActivateMouseActor();
        
        OnPhase3HorrorCompleted.Broadcast();
        
        CheckPhaseCompletion(Phase3Index);
    }
}

void ASLDeveloperBoss::LaunchNextPhase5Wall()
{
    if (!IsPlayerAlive())
    {
        bIsPhase5Active = false; 
        if (GetWorld() && Phase5WallAttackTimer.IsValid()) 
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase5WallAttackTimer);
        }
        return;
    }
    
    if (!bIsPhase5Active)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5 wall attack stopped - Phase5 not active"));
        if (GetWorld() && Phase5WallAttackTimer.IsValid()) 
        {
            GetWorld()->GetTimerManager().ClearTimer(Phase5WallAttackTimer);
        }
        return;
    }

    if (bPhase5EnableMultiWallAttack)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Launching Multi Wall Attack."));
        LaunchPhase5MultiWallAttack();
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Launching Single Wall Attack."));
        TArray<ASLLaunchableWall*> LaunchableWalls;
        for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
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
        LaunchPhase5SingleWall(SelectedWall);
    }
}

void ASLDeveloperBoss::OnPhase5MultiWallCompleted(ASLLaunchableWall* CompletedWall)
{
    if (!bIsPhase5Active)
    {
        return;
    }

    Phase5ActiveWalls.Remove(CompletedWall);
    
    if (!bPhase5EnableMultiWallAttack)
    {
        LaunchPhase5ReplacementWall();
    }
}

void ASLDeveloperBoss::OnPhase5WallHitMouseActor(ASLMouseActor* HitMouseActor, int32 WallPartIndex)
{
    if (!IsValid(HitMouseActor) || !bIsPhase5Active)
    {
        return;
    }
    
    HitMouseActor->ApplyWallStun(5.0f);
    OnPhase5MouseActorHit.Broadcast(HitMouseActor, WallPartIndex);
}

void ASLDeveloperBoss::OnPhase3AutoWallAttackTimer()
{
    if (!IsPlayerAlive())
    {
        StopPhase3AutoWallAttack(); 
        return;
    }
    
    if (!bIsPhase3Active || !bIsPhase3AutoWallAttackActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3 auto attack timer called but conditions not met"));
        return;
    }

    int32 Phase3Index = static_cast<int32>(EDeveloperBossPhase::Phase3_Horror);
    FPhaseLineData* Phase3Data = PhaseLineDataMap.Find(Phase3Index);
    
    if (!Phase3Data || Phase3Data->WallConnections.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: No wall connections found for auto attack"));
        StopPhase3AutoWallAttack();
        return;
    }

    ASLLaunchableWall* WallToLaunch = GetNextPhase3Wall();
    
    if (IsValid(WallToLaunch))
    {
        UE_LOG(LogTemp, Display, TEXT("🎯 Phase3 Auto Attack: Launching wall [%d]: %s"), 
               Phase3CurrentWallIndex, *WallToLaunch->GetName());
        
        // 기존 로직 사용
        PendingLineActivation.PhaseIndex = Phase3Index;
        PendingLineActivation.LaunchedWall = WallToLaunch;
        
        LaunchSpecificWall(WallToLaunch);

        ActivateConnectedLines(Phase3Index, WallToLaunch);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3 Auto Attack: No available wall found"));
    }

    // 다음 공격 예약
    if (bIsPhase3Active && bIsPhase3AutoWallAttackActive && IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            Phase3AutoWallAttackTimer,
            this,
            &ASLDeveloperBoss::OnPhase3AutoWallAttackTimer,
            Phase3AutoWallAttackInterval,
            false
        );
        
        UE_LOG(LogTemp, Display, TEXT("📅 Phase3: Next auto attack scheduled in %f seconds"), 
               Phase3AutoWallAttackInterval);
    }
}

void ASLDeveloperBoss::LaunchPhase5MultiWallAttack()
{
    // 활성화된 벽 정리 (무효한 참조 제거)
    CleanupInactiveWalls();
    
    // 활성 벽 개수 제한 확인
    if (bPhase5LimitActiveWalls && Phase5ActiveWalls.Num() >= Phase5MaxActiveWalls)
    {
        UE_LOG(LogTemp, Display, TEXT("Phase5: Cannot launch - too many active walls (%d/%d)"), 
               Phase5ActiveWalls.Num(), Phase5MaxActiveWalls);
        return;
    }
    
    TArray<ASLLaunchableWall*> LaunchableWalls;
    for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
    {
        if (IsValid(Wall) && Wall->CanLaunch() && !Phase5ActiveWalls.Contains(Wall))
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
    int32 MaxLaunchable = bPhase5LimitActiveWalls ? 
        FMath::Min(Phase5MaxActiveWalls - Phase5ActiveWalls.Num(), Phase5MaxSimultaneousWalls) : 
        Phase5MaxSimultaneousWalls;
    
    int32 WallsToLaunchCount = FMath::Min(MaxLaunchable, LaunchableWalls.Num());
    
    UE_LOG(LogTemp, Display, TEXT("Phase5: Launching %d walls (Active: %d/%d)"), 
           WallsToLaunchCount, Phase5ActiveWalls.Num(), Phase5MaxActiveWalls);

    for (int32 i = 0; i < WallsToLaunchCount; i++)
    {
        if (LaunchableWalls.Num() == 0) break;

        int32 RandomIndex = FMath::RandRange(0, LaunchableWalls.Num() - 1);
        ASLLaunchableWall* SelectedWall = LaunchableWalls[RandomIndex];
        LaunchableWalls.RemoveAt(RandomIndex);

        float RandomDelay = FMath::FRandRange(Phase5MultiWallDelayMin, Phase5MultiWallDelayMax);

        FTimerHandle WallTimer;
        GetWorldTimerManager().SetTimer(
            WallTimer,
            [this, SelectedWall]() { 
                if (IsValid(SelectedWall)) 
                {
                    LaunchPhase5SingleWall(SelectedWall);
                }
            },
            RandomDelay,
            false
        );
    }
}

void ASLDeveloperBoss::LaunchPhase5SingleWall(ASLLaunchableWall* Wall)
{
    if (!IsValid(Wall) || !bIsPhase5Active || !Wall->CanLaunch()) 
    {
        return;
    }
    
    // 활성 벽 개수 제한 확인
    if (Phase5ActiveWalls.Num() >= Phase5MaxActiveWalls)
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Phase5: Launching wall %s"), *Wall->GetName());

    // 활성 벽 목록에 추가
    Phase5ActiveWalls.AddUnique(Wall);
    
    Wall->OnAllWallPartsLaunched.AddUniqueDynamic(this, &ASLDeveloperBoss::OnPhase5MultiWallCompleted);
    Wall->OnWallHitMouseActor.AddUniqueDynamic(this, &ASLDeveloperBoss::OnPhase5WallHitMouseActor);

    Wall->LaunchWallToPlayer();
}

void ASLDeveloperBoss::LaunchPhase5ReplacementWall()
{
    if (!bIsPhase5Active)
    {
        return;
    }

    TArray<ASLLaunchableWall*> LaunchableWalls;
    for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
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

    float RandomDelay = FMath::FRandRange(Phase5MultiWallDelayMin, Phase5MultiWallDelayMax);
    
    FTimerHandle ReplacementTimer;
    GetWorldTimerManager().SetTimer(
        ReplacementTimer,
        [this, ReplacementWall]()
        {
            if (IsValid(ReplacementWall)) 
            {
                LaunchPhase5SingleWall(ReplacementWall);
            }
        },
        RandomDelay,
        false
    );
}

void ASLDeveloperBoss::ResetPhase5Wall(ASLLaunchableWall* WallToReset)
{
    if (!IsValid(WallToReset))
    {
        return;
    }
    WallToReset->OnAllWallPartsLaunched.RemoveAll(this);
    WallToReset->OnWallHitMouseActor.RemoveAll(this);
    
    WallToReset->ResetWall();
    
}

void ASLDeveloperBoss::SpawnPhase3MouseActor()
{
    if (IsValid(Phase3MouseActor))
    {
        DestroyPhase3MouseActor();
    }
    
    TSubclassOf<ASLMouseActor> Phase3Class = nullptr;
    if (IsValid(MouseActor))
    {
        Phase3Class = MouseActor->GetPhase3MouseActorClass();
    }
    
    if (!Phase3Class)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Phase3 Mouse Actor class specified"));
        return;
    }
    
    FVector SpawnLocation = MouseActor ? MouseActor->GetActorLocation() : GetActorLocation() + FVector(0, 0, 500.0f);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    Phase3MouseActor = GetWorld()->SpawnActor<ASLMouseActor>(Phase3Class, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandlePhase3MouseActorDestroyed);
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

void ASLDeveloperBoss::RegisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.AddDynamic(this, &ASLDeveloperBoss::HandleBossDeath);
    Boss->OnPatternFinished.AddDynamic(this, &ASLDeveloperBoss::HandlePatternFinished);
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

void ASLDeveloperBoss::SetupPhaseLines()
{
    for (auto& PhaseData : PhaseLineDataMap)
    {
        int32 PhaseIndex = PhaseData.Key;
        
        for (int32 i = 0; i < PhaseData.Value.Lines.Num(); i++)
        {
            ASLDeveloperRoomCable* Line = PhaseData.Value.Lines[i];
            if (IsValid(Line))
            {
                int32 UniqueLineIndex = PhaseIndex * 100 + i;
                Line->SetLineIndex(UniqueLineIndex);
                Line->OnBossLineDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandleLineDestroyed);
                Line->DeactivateLine();
            }
        }
    }
}

void ASLDeveloperBoss::StartPhasePattern(EDeveloperBossPhase Phase)
{
    switch (Phase)
    {
    case EDeveloperBossPhase::Phase0_Start:
        break;

    case EDeveloperBossPhase::Phase1_BossRush:
        StartPhase1BossRush();
        break;

    case EDeveloperBossPhase::Phase2_HackSlash:
        StartPhase2HackSlash();
        break;

    case EDeveloperBossPhase::Phase3_Horror:
        StartPhase3Horror();
        break;

    case EDeveloperBossPhase::Phase4_Platformer:
        StartPhase4Platformer();
        break;

    case EDeveloperBossPhase::Phase5_Final:
        StartPhase5Final();
        break;
    }
}

void ASLDeveloperBoss::ChangePhase(EDeveloperBossPhase NewPhase)
{
    if (CurrentPhase != NewPhase)
    {
        CurrentPhase = NewPhase;
        OnPhaseChanged.Broadcast(static_cast<int32>(NewPhase));
    }
}

void ASLDeveloperBoss::CheckPhaseCompletion(int32 PhaseIndex)
{
    if (IsPhaseCompleted(PhaseIndex))
    {
        OnPhaseCompleted.Broadcast(PhaseIndex);
        
        if (PhaseIndex == static_cast<int32>(CurrentPhase))
        {
            EDeveloperBossPhase NextPhase = static_cast<EDeveloperBossPhase>(PhaseIndex + 1);
            
            if (static_cast<int32>(NextPhase) <= static_cast<int32>(EDeveloperBossPhase::Phase5_Final))
            {
                ChangePhase(NextPhase);
                StartPhasePattern(NextPhase);
            }
            else
            {
                OnDeveloperBossPatternFinished.Broadcast();
            }
        }
    }
}

void ASLDeveloperBoss::LaunchSpecificWall(ASLLaunchableWall* Wall)
{
    if (!IsValid(Wall))
    {
        return;
    }

    ResetCurrentWall();
    CurrentWall = Wall;
    CurrentWall->OnAllWallPartsLaunched.AddDynamic(this, &ASLDeveloperBoss::HandleWallAttackFinished);
    CurrentWall->LaunchWallToPlayer();
    
    bCanLaunchWall = false;
    
    FTimerHandle CooldownTimer;
    GetWorldTimerManager().SetTimer(
        CooldownTimer,
        this,
        &ASLDeveloperBoss::OnWallCooldownFinishedInternal,
        WallAttackCooldown,
        false
    );
    
}

void ASLDeveloperBoss::StartPhase1BossRush()
{
    if (AvailableBossClasses.Num() == 0)
    {
        CompletePhase1BossRush();
        return;
    }

    bIsPhase1Active = true;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = AvailableBossClasses.Num();
    
    // 시작 시네마틱 재생
    PlayPhase1StartCinematic();
}

void ASLDeveloperBoss::SpawnNextPhase1Boss()
{
    if (!bIsPhase1Active || Phase1CurrentBossIndex >= AvailableBossClasses.Num())
    {
        return;
    }

    TSubclassOf<ASLAIBaseCharacter> BossClass = AvailableBossClasses[Phase1CurrentBossIndex];
    
    FVector SpawnLocation = GetActorLocation() + Phase1BossSpawnOffset;
    SpawnLocation += FVector(FMath::RandRange(-100.f, 100.f), FMath::RandRange(-100.f, 100.f), 0.f);
    
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
    
    ASLAIBaseCharacter* SpawnedBoss = SpawnBossCharacter(BossClass, SpawnTransform);
    
    if (SpawnedBoss)
    {
        OnBossSpawnCompleted.Broadcast(SpawnedBoss);
        WeakenBossForPhase1(SpawnedBoss);
        Phase1CurrentBossIndex++;
    }
    else
    {
        Phase1CurrentBossIndex++;
    }
}

void ASLDeveloperBoss::HandlePhase1BossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (!bIsPhase1Active)
    {
        return;
    }
    
    if (Phase1CurrentBossIndex < AvailableBossClasses.Num())
    {
        // 다음 보스 시네마틱 재생
        PlayPhase1BossCinematic(Phase1CurrentBossIndex);
    }
    else
    {
        if (SpawnedBosses.Num() == 0)
        {
            CompletePhase1BossRush();
        }
    }
}

void ASLDeveloperBoss::CompletePhase1BossRush()
{
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    CleanupDeadBosses();
    OnPhase1BossRushCompleted.Broadcast();
}

void ASLDeveloperBoss::WeakenBossForPhase1(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }

    float OriginalMaxHealth = Boss->GetMaxHealth();
    float WeakenedHealth = OriginalMaxHealth * Phase1BossHealthMultiplier;
    
    Boss->SetMaxHealth(WeakenedHealth);
    Boss->SetCurrentHealth(WeakenedHealth);
    Boss->SetIsSpecialPattern(false);
}

void ASLDeveloperBoss::StartPhase2HackSlash()
{
    if (!IsValid(Phase2Room))
    {
        return;
    }

    bIsPhase2Active = true;

    if (IsValid(MouseActor))
    {
        DeactivateMouseActor();
    }
    
    Phase2Room->OnRoomEscapeWallDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandlePhase2RoomEscape);
    Phase2Room->ActivateRoom();
    
    Phase2Room->TeleportPlayerToRoom();
    Phase2Room->SpawnAllNPCs();
}

void ASLDeveloperBoss::StartPhase3Horror()
{
    if (!IsValid(MouseActor))
    {
        return;
    }
    
    bIsPhase3Active = true;
    bIsPhase3WallAttackScheduled = false;
    ResetPhase3WallIndex();
    
    int32 Phase3Index = static_cast<int32>(EDeveloperBossPhase::Phase3_Horror);
    FPhaseLineData* Phase3Data = PhaseLineDataMap.Find(Phase3Index);
    if (Phase3Data)
    {
        for (int32 i = 0; i < Phase3Data->WallConnections.Num(); i++)
        {
            ASLLaunchableWall* Wall = Phase3Data->WallConnections[i].Wall;
            if (IsValid(Wall))
            {
                Wall->SetActorHiddenInGame(false);
                Wall->SetActorEnableCollision(true);
                Wall->SetActorTickEnabled(true);
                UE_LOG(LogTemp, Display, TEXT("Phase3: Reset wall [%d]: %s"), i, *Wall->GetName());
            }
        }
    }
    
    DeactivateMouseActor();
    SpawnPhase3MouseActor();
    StartPhase3AutoWallAttack();
    
}

void ASLDeveloperBoss::StartPhase4Platformer()
{
    bIsPhase4Active = true;
    
    UE_LOG(LogTemp, Display, TEXT("🏗️ Phase 4 Platformer Started"));
}

void ASLDeveloperBoss::StartPhase5Final()
{
    if (!IsValid(MouseActor))
    {
        OnDeveloperBossPatternFinished.Broadcast();
        return;
    }

    bIsPhase5Active = true;
    CurrentPhase5Wall = nullptr; 
    
    // Phase5 활성 벽 목록 초기화
    Phase5ActiveWalls.Empty();

    InitializePhase5WallAttack(); 
    
    // Phase 5에서만 벽 자동 리셋 활성화
    for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->SetAutoResetEnabled(true);
        }
    }

    if (IsValid(MouseActor))
    {
        MouseActor->DisableAttackability();
    }

    ActivateMouseActor();

    GetWorldTimerManager().SetTimer(
        Phase5WallAttackTimer, 
        this,
        &ASLDeveloperBoss::LaunchNextPhase5Wall,
        Phase5WallAttackInterval, 
        true,                     
        Phase5WallAttackDelay     
    );
}

void ASLDeveloperBoss::CleanupDeadBosses()
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

void ASLDeveloperBoss::ResetCurrentWall()
{
    if (IsValid(CurrentWall))
    {
        CurrentWall->OnAllWallPartsLaunched.RemoveAll(this);
        CurrentWall = nullptr;
    }
}

void ASLDeveloperBoss::DestroyPhase3MouseActor()
{
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.RemoveAll(this);
        Phase3MouseActor->Destroy();
        Phase3MouseActor = nullptr;
    }
}

void ASLDeveloperBoss::InitializePhase5WallAttack()
{
    Phase5AvailableWalls.Empty();

    for (auto& PhaseData : PhaseLineDataMap)
    {
        for (const FWallLineConnection& Connection : PhaseData.Value.WallConnections)
        {
            if (IsValid(Connection.Wall))
            {
                Phase5AvailableWalls.Add(Connection.Wall);
            }
        }
    }
    ResetAllWalls();
}

void ASLDeveloperBoss::ResetAllWalls()
{
    for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
    {
        if (IsValid(Wall))
        {
            Wall->ResetWall();
        }
    }
}

ASLLaunchableWall* ASLDeveloperBoss::GetNextPhase3Wall()
{
    int32 Phase3Index = static_cast<int32>(EDeveloperBossPhase::Phase3_Horror);
    FPhaseLineData* Phase3Data = PhaseLineDataMap.Find(Phase3Index);
    
    if (!Phase3Data || Phase3Data->WallConnections.Num() == 0)
    {
        return nullptr;
    }

    ASLLaunchableWall* SelectedWall = nullptr;

    if (bPhase3RandomWallSelection)
    {
        TArray<ASLLaunchableWall*> AvailableWalls;
        
        for (const FWallLineConnection& Connection : Phase3Data->WallConnections)
        {
            if (IsValid(Connection.Wall) && Connection.Wall->CanLaunch())
            {
                AvailableWalls.Add(Connection.Wall);
            }
        }
        
        if (AvailableWalls.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, AvailableWalls.Num() - 1);
            SelectedWall = AvailableWalls[RandomIndex];
        }
    }
    else
    {
        int32 WallCount = Phase3Data->WallConnections.Num();
        int32 CheckedWalls = 0;
        
        while (CheckedWalls < WallCount)
        {
            if (Phase3Data->WallConnections.IsValidIndex(Phase3CurrentWallIndex))
            {
                ASLLaunchableWall* Wall = Phase3Data->WallConnections[Phase3CurrentWallIndex].Wall;
                
                if (IsValid(Wall) && Wall->CanLaunch())
                {
                    SelectedWall = Wall;
                    Phase3CurrentWallIndex = (Phase3CurrentWallIndex + 1) % WallCount;
                    break;
                }
            }
            
            Phase3CurrentWallIndex = (Phase3CurrentWallIndex + 1) % WallCount;
            CheckedWalls++;
        }
    }

    return SelectedWall;
}

void ASLDeveloperBoss::ResetPhase3WallIndex()
{
    Phase3CurrentWallIndex = 0;
}

void ASLDeveloperBoss::CleanupInactiveWalls()
{
    Phase5ActiveWalls.RemoveAll([](ASLLaunchableWall* Wall) {
        return !IsValid(Wall) || Wall->CanLaunch(); // 리셋되어 다시 발사 가능한 상태면 비활성으로 간주
    });
}

void ASLDeveloperBoss::LaunchPhase4WallWithLines()
{
    if (!bIsPhase4Active)
    {
        return;
    }
    
    int32 Phase4Index = static_cast<int32>(EDeveloperBossPhase::Phase4_Platformer);
    FPhaseLineData* Phase4Data = PhaseLineDataMap.Find(Phase4Index);
    
    if (!Phase4Data || Phase4Data->WallConnections.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: No wall connections found"));
        return;
    }
    
    // 발사 가능한 벽 찾기
    ASLLaunchableWall* TargetWall = nullptr;
    for (const FWallLineConnection& Connection : Phase4Data->WallConnections)
    {
        if (IsValid(Connection.Wall) && Connection.Wall->CanLaunch())
        {
            TargetWall = Connection.Wall;
            break;
        }
    }
    
    if (IsValid(TargetWall))
    {
        UE_LOG(LogTemp, Display, TEXT("Phase4: Launching wall with immediate line activation: %s"), 
               *TargetWall->GetName());
        
        // 벽 발사
        LaunchSpecificWall(TargetWall);
        
        // 즉시 선 활성화
        ActivateConnectedLines(Phase4Index, TargetWall);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: No available walls to launch"));
    }
}

void ASLDeveloperBoss::StartPhase4FloorCollapse()
{
    if (!IsValid(Phase4FallingFloor) || !bIsPhase4Active)
    {
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase 4: Starting floor collapse"));
    Phase4FallingFloor->StartFloorCollapse();
}

void ASLDeveloperBoss::ResetPhase4Floor()
{
    if (IsValid(Phase4FallingFloor))
    {
        Phase4FallingFloor->ResetFloor();
    }
}

void ASLDeveloperBoss::HandlePhase4FloorCollapseCompleted()
{
    if (!bIsPhase4Active)
    {
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase 4: Floor collapse completed - Starting auto wall attacks"));
    
    OnPhase4PlatformerCompleted.Broadcast();
    
    if (IsValid(GetWorld()))
    {
        GetWorld()->GetTimerManager().SetTimer(
            Phase4AutoWallAttackTimer,
            [this]()
            {
                if (bIsPhase4Active)
                {
                    LaunchPhase4WallWithLines(); // 새 함수 사용
                    
                    // 다음 공격 예약 (4초마다)
                    if (bIsPhase4Active && IsValid(GetWorld()))
                    {
                        GetWorld()->GetTimerManager().SetTimer(
                            Phase4AutoWallAttackTimer,
                            [this]() { 
                                if (bIsPhase4Active) 
                                {
                                    LaunchPhase4WallWithLines(); // 여기도 새 함수 사용
                                }
                            },
                            4.0f,
                            true  // 반복
                        );
                    }
                }
            },
            2.0f,  // 2초 후 첫 공격
            false
        );
    }
}

// 바닥이 부서지길 원할때 호출할 함수
void ASLDeveloperBoss::TriggerPhase4FloorCollapse()
{
    if (!bIsPhase4Active)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase 4 not active! Call StartPhase4Platformer() first"));
        return;
    }
    
    if (!IsValid(Phase4FallingFloor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4FallingFloor not assigned"));
        return;
    }
    
    if (Phase4FloorCollapseDelay > 0.0f)
    {
        FTimerHandle Phase4DelayTimer;
        GetWorldTimerManager().SetTimer(
            Phase4DelayTimer,
            this,
            &ASLDeveloperBoss::StartPhase4FloorCollapse,
            Phase4FloorCollapseDelay,
            false
        );
    }
    else
    {
        StartPhase4FloorCollapse();
    }
}

bool ASLDeveloperBoss::IsPlayerAlive() const
{
    if (!GetWorld())
    {
        return false;
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController || !PlayerController->GetPawn())
    {
        return false;
    }

    if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(PlayerController->GetPawn()))
    {
        if (PlayerCharacter->IsInPrimaryState(TAG_Character_Dead))
        {
            return false;
        }
    }

    return true;
}

void ASLDeveloperBoss::PlayPhase1StartCinematic()
{
    if (Phase1Cinematics.IsValidIndex(0) && IsValid(Phase1Cinematics[0]))
    {
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* SequenceActor = nullptr;
        ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            Phase1Cinematics[0],
            PlaybackSettings,
            SequenceActor
        )->Play();
    }
    else
    {
        SpawnNextPhase1Boss();
    }
}

void ASLDeveloperBoss::PlayPhase1BossCinematic(int32 BossIndex)
{
    int32 CinematicIndex = BossIndex + 1;
    
    if (Phase1Cinematics.IsValidIndex(CinematicIndex) && IsValid(Phase1Cinematics[CinematicIndex]))
    {
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* SequenceActor = nullptr;
        ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            Phase1Cinematics[CinematicIndex],
            PlaybackSettings,
            SequenceActor
        )->Play();
    }
    else
    {
        SpawnNextPhase1Boss();
    }
}