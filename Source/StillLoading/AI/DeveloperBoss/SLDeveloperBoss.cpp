#include "SLDeveloperBoss.h"
#include "SLDeveloperBossPhase1.h"
#include "SLDeveloperBossPhase2.h"
#include "SLDeveloperBossPhase3.h"
#include "SLDeveloperBossPhase4.h"
#include "SLDeveloperBossPhase5.h"
#include "AI/Actors/SLDeveloperRoomCable.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "AI/Actors/SLMouseActor.h"
#include "AI/Actors/SLDeveloperRoomSpace.h"
#include "AI/Actors/SLPhase4FallingFloor.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Engine/World.h"
#include "TimerManager.h"

// 구조체 생성자들
FPendingLineActivation::FPendingLineActivation()
{
    PhaseIndex = -1;
    LaunchedWall = nullptr;
}

FWallLineConnection::FWallLineConnection()
{
    Wall = nullptr;
    ConnectedLineIndices.Empty();
}

FPhaseLineData::FPhaseLineData()
{
    Lines.Empty();
    WallConnections.Empty();
}

ASLDeveloperBoss::ASLDeveloperBoss()
{
    PrimaryActorTick.bCanEverTick = false;

    // Core State
    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    bIsFightStarted = false;
    bCanLaunchWall = true;
    
    // Wall System
    WallAttackCooldown = 3.0f;
    CurrentWall = nullptr;
    
    // Mouse Actor
    MouseActor = nullptr;
    MouseActorClass = nullptr;
    
    // Configuration
    ConfigDataAsset = nullptr;

    // Phase Actors (Lazy Loading - 초기값 nullptr)
    Phase1Actor = nullptr;
    Phase2Actor = nullptr;
    Phase3Actor = nullptr;
    Phase4Actor = nullptr;
    Phase5Actor = nullptr;
    CurrentPhaseActor = nullptr;
    
    // Phase Actor Classes
    Phase1ActorClass = nullptr;
    Phase2ActorClass = nullptr;
    Phase3ActorClass = nullptr;
    Phase4ActorClass = nullptr;
    Phase5ActorClass = nullptr;

    // Lazy Loading Settings
    MaxCachedPhases = 2;
    bEnablePhasePreloading = true;
    bAutoCleanupInactivePhases = true;

    // Level Actor References
    Phase2RoomSpace = nullptr;
    Phase4FallingFloor = nullptr;

    // Pending Line Activation
    PendingLineActivation.PhaseIndex = -1;
    PendingLineActivation.LaunchedWall = nullptr;
}

void ASLDeveloperBoss::BeginPlay()
{
    Super::BeginPlay();
    
    SetupPhaseLines();
    
}

void ASLDeveloperBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(CurrentPhaseActor))
    {
        CurrentPhaseActor->EndPhase();
    }
    
    // 모든 Phase Actor 정리
    DestroyAllInactivePhases();
    
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    
    ResetCurrentWall();
    
    Super::EndPlay(EndPlayReason);
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

void ASLDeveloperBoss::StartPhase(EDeveloperBossPhase PhaseType)
{
    // 현재 페이즈 검증
    if (CurrentPhaseActor && CurrentPhaseActor->IsPhaseActive())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start phase %d - Current phase %d is still active"), 
               static_cast<int32>(PhaseType), static_cast<int32>(CurrentPhase));
        return;
    }
    
    // 유효한 페이즈 타입 검증
    if (!IsValidPhaseType(PhaseType))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid phase type: %d"), static_cast<int32>(PhaseType));
        return;
    }
    
    // 이전 페이즈 정리
    if (CurrentPhaseActor && CurrentPhaseActor->IsPhaseActive())
    {
        CurrentPhaseActor->EndPhase();
    }
    
    // 새 페이즈 액터를 Lazy Loading으로 가져오기
    ASLDeveloperBossPhaseBase* NewPhaseActor = GetPhaseActor(PhaseType);
    
    if (!IsValid(NewPhaseActor))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get or create phase actor for phase %d"), 
               static_cast<int32>(PhaseType));
        return;
    }
    
    // 페이즈 전환
    CurrentPhaseActor = NewPhaseActor;
    CurrentPhase = PhaseType;
    
    OnPhaseChanged.Broadcast(static_cast<int32>(PhaseType));
    
    // 페이즈 시작
    CurrentPhaseActor->StartPhase();
    
    // 다음 페이즈 미리 로드 (선택적)
    if (bEnablePhasePreloading)
    {
        PreloadNextPhase();
    }
    
    // 비활성 페이즈 정리 (선택적)
    if (bAutoCleanupInactivePhases)
    {
        DestroyAllInactivePhases();
    }
    
    UE_LOG(LogTemp, Display, TEXT("Successfully started Phase %d (Lazy Loaded)"), static_cast<int32>(PhaseType));
}

ASLDeveloperBossPhaseBase* ASLDeveloperBoss::GetPhaseActor(EDeveloperBossPhase PhaseType)
{
    // 액세스 순서 업데이트 (LRU 캐시)
    PhaseAccessOrder.Remove(PhaseType);
    PhaseAccessOrder.Add(PhaseType);
    
    // 캐시 크기 관리
    if (PhaseAccessOrder.Num() > MaxCachedPhases)
    {
        EDeveloperBossPhase OldestPhase = PhaseAccessOrder[0];
        if (OldestPhase != PhaseType) // 현재 페이즈가 아닌 경우만 제거
        {
            DestroyPhaseActor(OldestPhase);
            PhaseAccessOrder.RemoveAt(0);
        }
    }
    
    switch (PhaseType)
    {
    case EDeveloperBossPhase::Phase1_BossRush:
        return GetOrCreatePhase1();
    case EDeveloperBossPhase::Phase2_HackSlash:
        return GetOrCreatePhase2();
    case EDeveloperBossPhase::Phase3_Horror:
        return GetOrCreatePhase3();
    case EDeveloperBossPhase::Phase4_Platformer:
        return GetOrCreatePhase4();
    case EDeveloperBossPhase::Phase5_Final:
        return GetOrCreatePhase5();
    default:
        return nullptr;
    }
}

ASLDeveloperBossPhase1* ASLDeveloperBoss::GetOrCreatePhase1()
{
    if (!IsValid(Phase1Actor))
    {
        UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Creating Phase1 Actor"));
        Phase1Actor = CreatePhaseActor<ASLDeveloperBossPhase1>(Phase1ActorClass);
        
        if (IsValid(Phase1Actor) && ConfigDataAsset)
        {
            SetupPhase1Actor(Phase1Actor, ConfigDataAsset->Phase1Config);
        }
    }
    return Phase1Actor;
}

ASLDeveloperBossPhase2* ASLDeveloperBoss::GetOrCreatePhase2()
{
    if (!IsValid(Phase2Actor))
    {
        UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Creating Phase2 Actor"));
        Phase2Actor = CreatePhaseActor<ASLDeveloperBossPhase2>(Phase2ActorClass);
        
        if (IsValid(Phase2Actor) && ConfigDataAsset)
        {
            SetupPhase2Actor(Phase2Actor, ConfigDataAsset->Phase2Config);
        }
    }
    return Phase2Actor;
}

ASLDeveloperBossPhase3* ASLDeveloperBoss::GetOrCreatePhase3()
{
    if (!IsValid(Phase3Actor))
    {
        UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Creating Phase3 Actor"));
        Phase3Actor = CreatePhaseActor<ASLDeveloperBossPhase3>(Phase3ActorClass);
        
        if (IsValid(Phase3Actor) && ConfigDataAsset)
        {
            SetupPhase3Actor(Phase3Actor, ConfigDataAsset->Phase3Config);
            SetupPhase3Walls(Phase3Actor);
        }
    }
    return Phase3Actor;
}

ASLDeveloperBossPhase4* ASLDeveloperBoss::GetOrCreatePhase4()
{
    if (!IsValid(Phase4Actor))
    {
        UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Creating Phase4 Actor"));
        Phase4Actor = CreatePhaseActor<ASLDeveloperBossPhase4>(Phase4ActorClass);
        
        if (IsValid(Phase4Actor) && ConfigDataAsset)
        {
            SetupPhase4Actor(Phase4Actor, ConfigDataAsset->Phase4Config);
        }
    }
    return Phase4Actor;
}

ASLDeveloperBossPhase5* ASLDeveloperBoss::GetOrCreatePhase5()
{
    if (!IsValid(Phase5Actor))
    {
        UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Creating Phase5 Actor"));
        Phase5Actor = CreatePhaseActor<ASLDeveloperBossPhase5>(Phase5ActorClass);
        
        if (IsValid(Phase5Actor) && ConfigDataAsset)
        {
            SetupPhase5Actor(Phase5Actor, ConfigDataAsset->Phase5Config);
        }
    }
    return Phase5Actor;
}

template<typename T>
T* ASLDeveloperBoss::CreatePhaseActor(TSubclassOf<T> PhaseClass)
{
    if (!PhaseClass || !GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot create phase actor - Invalid class or world"));
        return nullptr;
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    T* NewPhaseActor = GetWorld()->SpawnActor<T>(PhaseClass, GetActorTransform(), SpawnParams);
    
    if (IsValid(NewPhaseActor))
    {
        NewPhaseActor->SetOwnerBoss(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create phase actor"));
    }
    
    return NewPhaseActor;
}

void ASLDeveloperBoss::DestroyPhaseActor(EDeveloperBossPhase PhaseType)
{
    ASLDeveloperBossPhaseBase* ActorToDestroy = FindExistingPhaseActor(PhaseType);
    
    if (!IsValid(ActorToDestroy))
    {
        return;
    }
    
    // 현재 활성 페이즈는 파괴하지 않음
    if (ActorToDestroy == CurrentPhaseActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot destroy current active phase: %d"), static_cast<int32>(PhaseType));
        return;
    }
    
    // 페이즈가 활성 상태면 먼저 종료
    if (ActorToDestroy->IsPhaseActive())
    {
        ActorToDestroy->EndPhase();
    }
    
    UE_LOG(LogTemp, Display, TEXT(" Lazy Loading: Destroying Phase %d Actor"), static_cast<int32>(PhaseType));
    
    // 이벤트 바인딩 해제
    ActorToDestroy->OnPhaseCompleted.RemoveAll(this);
    
    // 액터 파괴
    ActorToDestroy->Destroy();
    
    // 참조 제거
    switch (PhaseType)
    {
    case EDeveloperBossPhase::Phase1_BossRush:
        Phase1Actor = nullptr;
        break;
    case EDeveloperBossPhase::Phase2_HackSlash:
        Phase2Actor = nullptr;
        break;
    case EDeveloperBossPhase::Phase3_Horror:
        Phase3Actor = nullptr;
        break;
    case EDeveloperBossPhase::Phase4_Platformer:
        Phase4Actor = nullptr;
        break;
    case EDeveloperBossPhase::Phase5_Final:
        Phase5Actor = nullptr;
        break;
    }
    
    // 액세스 순서에서도 제거
    PhaseAccessOrder.Remove(PhaseType);
}

void ASLDeveloperBoss::DestroyAllInactivePhases()
{
    TArray<EDeveloperBossPhase> PhasesToDestroy;
    
    // 현재 활성 페이즈가 아닌 모든 페이즈 수집
    if (IsValid(Phase1Actor) && Phase1Actor != CurrentPhaseActor)
        PhasesToDestroy.Add(EDeveloperBossPhase::Phase1_BossRush);
    if (IsValid(Phase2Actor) && Phase2Actor != CurrentPhaseActor)
        PhasesToDestroy.Add(EDeveloperBossPhase::Phase2_HackSlash);
    if (IsValid(Phase3Actor) && Phase3Actor != CurrentPhaseActor)
        PhasesToDestroy.Add(EDeveloperBossPhase::Phase3_Horror);
    if (IsValid(Phase4Actor) && Phase4Actor != CurrentPhaseActor)
        PhasesToDestroy.Add(EDeveloperBossPhase::Phase4_Platformer);
    if (IsValid(Phase5Actor) && Phase5Actor != CurrentPhaseActor)
        PhasesToDestroy.Add(EDeveloperBossPhase::Phase5_Final);
    
    for (EDeveloperBossPhase PhaseType : PhasesToDestroy)
    {
        DestroyPhaseActor(PhaseType);
    }
    
    UE_LOG(LogTemp, Display, TEXT(" Cleaned up %d inactive phase actors"), PhasesToDestroy.Num());
}

void ASLDeveloperBoss::PreloadNextPhase()
{
    EDeveloperBossPhase NextPhase = GetNextPhase(CurrentPhase);
    
    if (NextPhase != EDeveloperBossPhase::Phase0_Start)
    {
        UE_LOG(LogTemp, Display, TEXT(" Preloading next phase: %d"), static_cast<int32>(NextPhase));
        GetPhaseActor(NextPhase); // 백그라운드에서 미리 생성
    }
}

void ASLDeveloperBoss::TriggerFirstWallDuringDialogue()
{
    UE_LOG(LogTemp, Warning, TEXT(" TriggerFirstWallDuringDialogue called"));
    
    if (bIsFightStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("️ Fight already started"));
        return;
    }

    bIsFightStarted = true;
    
    // Phase 1 시작
    UE_LOG(LogTemp, Warning, TEXT(" Starting Phase 1"));
    StartPhase(EDeveloperBossPhase::Phase1_BossRush);
    
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

    UE_LOG(LogTemp, Display, TEXT(" ActivateConnectedLines: Phase %d, Wall %s"), 
           PhaseIndex, *LaunchedWall->GetName());

    bool bFoundConnection = false;
    for (int32 i = 0; i < PhaseData->WallConnections.Num(); i++)
    {
        const FWallLineConnection& Connection = PhaseData->WallConnections[i];
        
        if (Connection.Wall == LaunchedWall)
        {
            bFoundConnection = true;
            UE_LOG(LogTemp, Display, TEXT("    Found matching connection! Activating %d lines"), 
                   Connection.ConnectedLineIndices.Num());
            
            for (int32 LineIndex : Connection.ConnectedLineIndices)
            {
                if (PhaseData->Lines.IsValidIndex(LineIndex))
                {
                    ASLDeveloperRoomCable* LineToActivate = PhaseData->Lines[LineIndex];
                    if (IsValid(LineToActivate))
                    {
                        LineToActivate->ActivateLine();
                        UE_LOG(LogTemp, Display, TEXT("    Activated line %d in phase %d"), LineIndex, PhaseIndex);
                    }
                }
            }
            return;
        }
    }
    
    if (!bFoundConnection)
    {
        UE_LOG(LogTemp, Warning, TEXT(" No connection found for wall %s in phase %d"), 
               *LaunchedWall->GetName(), PhaseIndex);
    }
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

void ASLDeveloperBoss::TestKillAllBosses()
{
    if (IsValid(Phase1Actor))
    {
        Phase1Actor->TestKillAllBosses();
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
        UE_LOG(LogTemp, Error, TEXT(" Phase %d NOT FOUND in PhaseLineDataMap!"), PhaseIndex);
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT(" Phase %d found"), PhaseIndex);
    UE_LOG(LogTemp, Display, TEXT("Lines count: %d"), PhaseData->Lines.Num());
    UE_LOG(LogTemp, Display, TEXT("Wall connections count: %d"), PhaseData->WallConnections.Num());
}

void ASLDeveloperBoss::DebugCurrentState() const
{
    UE_LOG(LogTemp, Warning, TEXT("=== DEVELOPER BOSS DEBUG STATE ==="));
    UE_LOG(LogTemp, Warning, TEXT("Current Phase: %d"), static_cast<int32>(CurrentPhase));
    UE_LOG(LogTemp, Warning, TEXT("Fight Started: %s"), bIsFightStarted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("Can Launch Wall: %s"), bCanLaunchWall ? TEXT("YES") : TEXT("NO"));
    
    if (IsValid(CurrentPhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Current Phase Actor: %s (Active: %s)"), 
               *CurrentPhaseActor->GetClass()->GetName(),
               CurrentPhaseActor->IsPhaseActive() ? TEXT("YES") : TEXT("NO"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Current Phase Actor: NULL"));
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: %s"), 
           IsValid(MouseActor) ? *MouseActor->GetClass()->GetName() : TEXT("NULL"));
    
    UE_LOG(LogTemp, Warning, TEXT("===================================="));
}

bool ASLDeveloperBoss::IsPhase1Active() const
{
    return IsValid(Phase1Actor) && Phase1Actor->IsPhaseActive();
}

int32 ASLDeveloperBoss::GetPhase1BossesRemaining() const
{
    if (IsValid(Phase1Actor))
    {
        return Phase1Actor->GetBossesRemaining();
    }
    return 0;
}

void ASLDeveloperBoss::SpawnNextPhase1Boss()
{
    if (IsValid(Phase1Actor))
    {
        Phase1Actor->SpawnNextBoss();
    }
}

void ASLDeveloperBoss::StartPhase3AutoWallAttack()
{
    if (IsValid(Phase3Actor))
    {
        Phase3Actor->StartAutoWallAttack();
    }
}

void ASLDeveloperBoss::StopPhase3AutoWallAttack()
{
    if (IsValid(Phase3Actor))
    {
        Phase3Actor->StopAutoWallAttack();
    }
}

bool ASLDeveloperBoss::IsPhase3AutoWallAttackActive() const
{
    if (IsValid(Phase3Actor))
    {
        return Phase3Actor->IsAutoWallAttackActive();
    }
    return false;
}

void ASLDeveloperBoss::StartPhase4FloorCollapse()
{
    if (IsValid(Phase4Actor))
    {
        Phase4Actor->StartFloorCollapse();
    }
}

void ASLDeveloperBoss::ResetPhase4Floor()
{
    if (IsValid(Phase4Actor))
    {
        Phase4Actor->ResetFloor();
    }
}

void ASLDeveloperBoss::TriggerPhase4FloorCollapse()
{
    if (IsValid(Phase4Actor))
    {
        Phase4Actor->TriggerFloorCollapse();
    }
}

void ASLDeveloperBoss::HandlePhaseCompleted()
{
    if (!IsValid(CurrentPhaseActor))
    {
        return;
    }
    
    int32 CompletedPhaseIndex = CurrentPhaseActor->GetPhaseIndex();
    OnPhaseCompleted.Broadcast(CompletedPhaseIndex);
    
    // 다음 페이즈로 진행
    EDeveloperBossPhase NextPhase = static_cast<EDeveloperBossPhase>(CompletedPhaseIndex + 1);
    
    if (static_cast<int32>(NextPhase) <= static_cast<int32>(EDeveloperBossPhase::Phase5_Final))
    {
        ChangePhase(NextPhase);
    }
    else
    {
        OnDeveloperBossPatternFinished.Broadcast();
    }
}

void ASLDeveloperBoss::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (DeadBoss)
    {
        OnBossCharacterDeath.Broadcast(DeadBoss);
        
        if (IsValid(CurrentPhaseActor))
        {
            CurrentPhaseActor->HandleBossDeath(DeadBoss);
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
    
    if (IsValid(CurrentPhaseActor))
    {
        CurrentPhaseActor->HandleLineDestroyed(LineIndex);
    }
}

void ASLDeveloperBoss::HandleWallAttackFinished(ASLLaunchableWall* LaunchedWall)
{
    UE_LOG(LogTemp, Display, TEXT(" HandleWallAttackFinished called"));
    
    if (PendingLineActivation.LaunchedWall.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("   Processing pending line activation for Phase %d"), 
               PendingLineActivation.PhaseIndex);
        
        ActivateConnectedLines(PendingLineActivation.PhaseIndex, PendingLineActivation.LaunchedWall.Get());
        
        PendingLineActivation.PhaseIndex = -1;
        PendingLineActivation.LaunchedWall = nullptr;
    }
    
    ResetCurrentWall();
    
    if (IsValid(CurrentPhaseActor))
    {
        CurrentPhaseActor->HandleWallAttackFinished(LaunchedWall);
    }
}

void ASLDeveloperBoss::HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == MouseActor)
    {
        MouseActor = nullptr;
        OnDeveloperBossPatternFinished.Broadcast();
    }
}

void ASLDeveloperBoss::OnWallCooldownFinishedInternal()
{
    bCanLaunchWall = true;
    OnWallCooldownFinished.Broadcast();
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
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimer,
        this,
        &ASLDeveloperBoss::OnWallCooldownFinishedInternal,
        WallAttackCooldown,
        false
    );
}

void ASLDeveloperBoss::ResetCurrentWall()
{
    if (IsValid(CurrentWall))
    {
        CurrentWall->OnAllWallPartsLaunched.RemoveAll(this);
        CurrentWall = nullptr;
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

//  수정된 Setup 함수들
void ASLDeveloperBoss::SetupPhase1Actor(ASLDeveloperBossPhase1* PhaseActor, const FSLPhase1Config& Config)
{
    if (!IsValid(PhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1Actor is not valid"));
        return;
    }
    
    PhaseActor->SetOwnerBoss(this);
    
    PhaseActor->OnPhaseCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    PhaseActor->OnPhaseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    
    PhaseActor->OnBossSpawnCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandleBossDeath);
    PhaseActor->OnBossSpawnCompleted.AddDynamic(this, &ASLDeveloperBoss::HandleBossDeath);
    
    PhaseActor->SetConfig(Config);
    
    UE_LOG(LogTemp, Display, TEXT("Phase1 Actor setup completed"));
}

void ASLDeveloperBoss::SetupPhase2Actor(ASLDeveloperBossPhase2* PhaseActor, const FSLPhase2Config& Config)
{
    if (!IsValid(PhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase2Actor is not valid"));
        return;
    }
    
    PhaseActor->SetOwnerBoss(this);
    
    PhaseActor->OnPhaseCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    PhaseActor->OnPhaseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    
    PhaseActor->SetConfig(Config);
    
    if (IsValid(Phase2RoomSpace))
    {
        PhaseActor->SetRoomSpace(Phase2RoomSpace);
        UE_LOG(LogTemp, Display, TEXT("Phase2: Room Space assigned - %s"), *Phase2RoomSpace->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase2: No Room Space assigned"));
    }
    
    PhaseActor->SetMouseActor(MouseActor);
    
    UE_LOG(LogTemp, Display, TEXT("Phase2 Actor setup completed"));
}

void ASLDeveloperBoss::SetupPhase3Actor(ASLDeveloperBossPhase3* PhaseActor, const FSLPhase3Config& Config)
{
    if (!IsValid(PhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3Actor is not valid"));
        return;
    }
    
    PhaseActor->SetOwnerBoss(this);
    
    PhaseActor->OnPhaseCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    PhaseActor->OnPhaseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    
    PhaseActor->SetConfig(Config);
    
    PhaseActor->SetMouseActor(MouseActor);
    
    UE_LOG(LogTemp, Display, TEXT("Phase3 Actor setup completed"));
}

void ASLDeveloperBoss::SetupPhase4Actor(ASLDeveloperBossPhase4* PhaseActor, const FSLPhase4Config& Config)
{
    if (!IsValid(PhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4Actor is not valid"));
        return;
    }
    
    PhaseActor->SetOwnerBoss(this);
    
    PhaseActor->OnPhaseCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    PhaseActor->OnPhaseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    
    PhaseActor->SetConfig(Config);
    
    if (IsValid(Phase4FallingFloor))
    {
        PhaseActor->SetFallingFloor(Phase4FallingFloor);
        UE_LOG(LogTemp, Display, TEXT("Phase4: Falling Floor assigned - %s"), *Phase4FallingFloor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: No Falling Floor assigned"));
    }
    
    SetupPhase4Walls(PhaseActor);
    
    UE_LOG(LogTemp, Display, TEXT("Phase4 Actor setup completed"));
}

void ASLDeveloperBoss::SetupPhase5Actor(ASLDeveloperBossPhase5* PhaseActor, const FSLPhase5Config& Config)
{
    if (!IsValid(PhaseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase5Actor is not valid"));
        return;
    }
    
    PhaseActor->SetOwnerBoss(this);
    
    PhaseActor->OnPhaseCompleted.RemoveDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    PhaseActor->OnPhaseCompleted.AddDynamic(this, &ASLDeveloperBoss::HandlePhaseCompleted);
    
    PhaseActor->SetConfig(Config);
    
    PhaseActor->SetMouseActor(MouseActor);
    
    // 벽 설정
    SetupPhase5Walls(PhaseActor);
    
    UE_LOG(LogTemp, Display, TEXT("Phase5 Actor setup completed"));
}

void ASLDeveloperBoss::SetupPhase3Walls(ASLDeveloperBossPhase3* PhaseActor)
{
    if (!IsValid(PhaseActor))
    {
        return;
    }
    
    TArray<ASLLaunchableWall*> Phase3Walls;
    if (PhaseLineDataMap.Contains(3))
    {
        const FPhaseLineData& Phase3Data = PhaseLineDataMap[3];
        for (const FWallLineConnection& Connection : Phase3Data.WallConnections)
        {
            if (IsValid(Connection.Wall))
            {
                Phase3Walls.Add(Connection.Wall);
            }
        }
        UE_LOG(LogTemp, Display, TEXT("Phase3: Found %d walls from PhaseLineDataMap"), Phase3Walls.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: No PhaseLineDataMap entry found for phase 3"));
    }
    
    PhaseActor->SetAvailableWalls(Phase3Walls);
}

void ASLDeveloperBoss::SetupPhase4Walls(ASLDeveloperBossPhase4* PhaseActor)
{
    if (!IsValid(PhaseActor))
    {
        return;
    }
    
    TArray<ASLLaunchableWall*> Phase4Walls;
    if (PhaseLineDataMap.Contains(4))
    {
        const FPhaseLineData& Phase4Data = PhaseLineDataMap[4];
        for (const FWallLineConnection& Connection : Phase4Data.WallConnections)
        {
            if (IsValid(Connection.Wall))
            {
                Phase4Walls.Add(Connection.Wall);
            }
        }
        UE_LOG(LogTemp, Display, TEXT("Phase4: Found %d walls from PhaseLineDataMap"), Phase4Walls.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase4: No PhaseLineDataMap entry found for phase 4"));
    }
    
    PhaseActor->SetAvailableWalls(Phase4Walls);
}

void ASLDeveloperBoss::SetupPhase5Walls(ASLDeveloperBossPhase5* PhaseActor)
{
    if (!IsValid(PhaseActor))
    {
        return;
    }
    
    TArray<ASLLaunchableWall*> WallsToUse;
    
    // 1순위: 직접 설정된 벽들
    if (Phase5AvailableWalls.Num() > 0)
    {
        for (ASLLaunchableWall* Wall : Phase5AvailableWalls)
        {
            if (IsValid(Wall))
            {
                WallsToUse.Add(Wall);
            }
        }
        UE_LOG(LogTemp, Display, TEXT("Phase5: Using %d directly assigned walls"), WallsToUse.Num());
    }
    else
    {
        // 2순위: PhaseLineDataMap의 모든 벽 (기존 로직)
        for (auto& PhaseData : PhaseLineDataMap)
        {
            for (const FWallLineConnection& Connection : PhaseData.Value.WallConnections)
            {
                if (IsValid(Connection.Wall))
                {
                    WallsToUse.AddUnique(Connection.Wall);
                }
            }
        }
        UE_LOG(LogTemp, Display, TEXT("Phase5: Using %d walls from all phases"), WallsToUse.Num());
    }
    
    PhaseActor->SetAvailableWalls(WallsToUse);
}

bool ASLDeveloperBoss::IsValidPhaseType(EDeveloperBossPhase PhaseType) const
{
    return PhaseType >= EDeveloperBossPhase::Phase1_BossRush && 
           PhaseType <= EDeveloperBossPhase::Phase5_Final;
}

ASLDeveloperBossPhaseBase* ASLDeveloperBoss::FindExistingPhaseActor(EDeveloperBossPhase PhaseType) const
{
    switch (PhaseType)
    {
    case EDeveloperBossPhase::Phase1_BossRush:
        return Phase1Actor;
    case EDeveloperBossPhase::Phase2_HackSlash:
        return Phase2Actor;
    case EDeveloperBossPhase::Phase3_Horror:
        return Phase3Actor;
    case EDeveloperBossPhase::Phase4_Platformer:
        return Phase4Actor;
    case EDeveloperBossPhase::Phase5_Final:
        return Phase5Actor;
    default:
        return nullptr;
    }
}

bool ASLDeveloperBoss::ValidatePhaseActors() const
{
    bool bAllValid = true;
    
    if (!IsValid(Phase1Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase1 Actor is not valid"));
        bAllValid = false;
    }
    
    if (!IsValid(Phase2Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase2 Actor is not valid"));
        bAllValid = false;
    }
    
    if (!IsValid(Phase3Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase3 Actor is not valid"));
        bAllValid = false;
    }
    
    if (!IsValid(Phase4Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase4 Actor is not valid"));
        bAllValid = false;
    }
    
    if (!IsValid(Phase5Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase5 Actor is not valid"));
        bAllValid = false;
    }
    
    return bAllValid;
}

EDeveloperBossPhase ASLDeveloperBoss::GetNextPhase(EDeveloperBossPhase InCurrentPhase) const
{
    int32 NextPhaseIndex = static_cast<int32>(InCurrentPhase) + 1;
    
    if (NextPhaseIndex <= static_cast<int32>(EDeveloperBossPhase::Phase5_Final))
    {
        return static_cast<EDeveloperBossPhase>(NextPhaseIndex);
    }
    
    return EDeveloperBossPhase::Phase0_Start; // 마지막 페이즈 이후
}

void ASLDeveloperBoss::ChangePhase(EDeveloperBossPhase NewPhase)
{
    if (CurrentPhase != NewPhase)
    {
        StartPhase(NewPhase);
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

ASLDeveloperBossPhaseBase* ASLDeveloperBoss::GetCurrentPhaseActor() const
{
    return CurrentPhaseActor;
}