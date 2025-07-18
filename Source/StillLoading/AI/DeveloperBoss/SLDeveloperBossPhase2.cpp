#include "SLDeveloperBossPhase2.h"
#include "AI/Actors/SLDeveloperRoomSpace.h"
#include "AI/Actors/SLMouseActor.h"

ASLDeveloperBossPhase2::ASLDeveloperBossPhase2()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase2_HackSlash;
    RoomSpace = nullptr;
    MouseActor = nullptr;
    bIsCompleted = false;
}

void ASLDeveloperBossPhase2::BeginPlay()
{
    Super::BeginPlay();
    
    // Config에서 RoomSpace 설정
    if (Config.RoomSpace)
    {
        SetRoomSpace(Config.RoomSpace);
    }
}

void ASLDeveloperBossPhase2::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsValid(RoomSpace))
    {
        RoomSpace->OnRoomEscapeWallDestroyed.RemoveAll(this);
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase2::StartPhase()
{
    Super::StartPhase();
    
    if (!IsValid(RoomSpace))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase2: No room space configured"));
        bIsCompleted = true;
        CheckPhaseCompletion();
        return;
    }

    bIsCompleted = false;

    // 마우스 액터 비활성화
    if (IsValid(MouseActor))
    {
        MouseActor->StopOrbiting();
        MouseActor->SetActorHiddenInGame(true);
        MouseActor->SetActorEnableCollision(false);
    }
    
    // 방 활성화 및 플레이어 텔레포트
    RoomSpace->ActivateRoom();
    RoomSpace->TeleportPlayerToRoom();
    RoomSpace->SpawnAllNPCs();
}

void ASLDeveloperBossPhase2::EndPhase()
{
    if (IsValid(RoomSpace))
    {
        RoomSpace->CleanupNPCs();
        RoomSpace->DeactivateRoom();
    }

    // 마우스 액터 재활성화
    if (IsValid(MouseActor))
    {
        MouseActor->SetActorHiddenInGame(false);
        MouseActor->SetActorEnableCollision(true);
        MouseActor->StartOrbiting();
    }
    
    Super::EndPhase();
}

bool ASLDeveloperBossPhase2::IsPhaseCompleted() const
{
    return bIsCompleted;
}

void ASLDeveloperBossPhase2::SetConfig(const FSLPhase2Config& InConfig)
{
    Config = InConfig;
    
    // Config에서 RoomSpace 설정
    if (Config.RoomSpace)
    {
        SetRoomSpace(Config.RoomSpace);
    }
}

void ASLDeveloperBossPhase2::SetRoomSpace(ASLDeveloperRoomSpace* InRoomSpace)
{
    if (IsValid(RoomSpace))
    {
        RoomSpace->OnRoomEscapeWallDestroyed.RemoveAll(this);
    }
    
    RoomSpace = InRoomSpace;
    Config.RoomSpace = InRoomSpace;
    
    if (IsValid(RoomSpace))
    {
        RoomSpace->OnRoomEscapeWallDestroyed.AddDynamic(this, &ASLDeveloperBossPhase2::HandleRoomEscape);
    }
}

void ASLDeveloperBossPhase2::SetMouseActor(ASLMouseActor* InMouseActor)
{
    MouseActor = InMouseActor;
}

void ASLDeveloperBossPhase2::OnPhaseStarted()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 2 Hack & Slash Started"));
}

void ASLDeveloperBossPhase2::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 2 Hack & Slash Completed"));
}

void ASLDeveloperBossPhase2::HandleRoomEscape(ASLDeveloperRoomSpace* Room)
{
    if (!bIsPhaseActive || Room != RoomSpace)
    {
        return;
    }
    
    bIsCompleted = true;
    CheckPhaseCompletion();
}