#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBoss.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASLDeveloperBossPhaseBase::ASLDeveloperBossPhaseBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase0_Start;
    OwnerBoss = nullptr;
    bIsPhaseActive = false;
    PhaseIndex = 0;
}

void ASLDeveloperBossPhaseBase::BeginPlay()
{
    Super::BeginPlay();
    
    PhaseIndex = static_cast<int32>(PhaseType);
}

void ASLDeveloperBossPhaseBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsPhaseActive)
    {
        EndPhase();
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhaseBase::StartPhase()
{
    if (bIsPhaseActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase %d already active"), PhaseIndex);
        return;
    }
    
    bIsPhaseActive = true;
    OnPhaseStateChanged.Broadcast(true);
    OnPhaseStarted();
    
    UE_LOG(LogTemp, Display, TEXT("Phase %d started"), PhaseIndex);
}

void ASLDeveloperBossPhaseBase::EndPhase()
{
    if (!bIsPhaseActive)
    {
        return;
    }
    
    bIsPhaseActive = false;
    OnPhaseStateChanged.Broadcast(false);
    OnPhaseEnded();
    
    UE_LOG(LogTemp, Display, TEXT("Phase %d ended"), PhaseIndex);
}

bool ASLDeveloperBossPhaseBase::IsPhaseCompleted() const
{
    return false;
}

void ASLDeveloperBossPhaseBase::HandleLineDestroyed(int32 LineIndex)
{
    CheckPhaseCompletion();
}

void ASLDeveloperBossPhaseBase::HandleBossDeath(ASLAIBaseCharacter* DeadBoss)
{
    CheckPhaseCompletion();
}

void ASLDeveloperBossPhaseBase::HandleWallAttackFinished(ASLLaunchableWall* LaunchedWall)
{
    // Base implementation - do nothing
}

void ASLDeveloperBossPhaseBase::SetOwnerBoss(ASLDeveloperBoss* InOwnerBoss)
{
    OwnerBoss = InOwnerBoss;
}

ASLDeveloperBoss* ASLDeveloperBossPhaseBase::GetOwnerBoss() const
{
    return OwnerBoss;
}

bool ASLDeveloperBossPhaseBase::IsPhaseActive() const
{
    return bIsPhaseActive;
}

int32 ASLDeveloperBossPhaseBase::GetPhaseIndex() const
{
    return PhaseIndex;
}

void ASLDeveloperBossPhaseBase::OnPhaseStarted()
{
    // Override in derived classes
}

void ASLDeveloperBossPhaseBase::OnPhaseEnded()
{
    // Override in derived classes
}

void ASLDeveloperBossPhaseBase::CheckPhaseCompletion()
{
    if (IsPhaseCompleted())
    {
        OnPhaseCompleted.Broadcast();
    }
}