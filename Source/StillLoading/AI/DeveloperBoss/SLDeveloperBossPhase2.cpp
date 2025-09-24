#include "SLDeveloperBossPhase2.h"
#include "AI/Actors/SLDeveloperRoomSpace.h"
#include "AI/Actors/SLMouseActor.h"
#include "LevelSequencePlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASLDeveloperBossPhase2::ASLDeveloperBossPhase2()
{
    PrimaryActorTick.bCanEverTick = false;
    
    PhaseType = EDeveloperBossPhase::Phase2_HackSlash;
    RoomSpace = nullptr;
    MouseActor = nullptr;
    CurrentSequencePlayer = nullptr;
    bIsCompleted = false;
    bWaitingForCinematic = false;
    CurrentCinematicType = EPhase2CinematicType::Start;
}

void ASLDeveloperBossPhase2::BeginPlay()
{
    Super::BeginPlay();
}

void ASLDeveloperBossPhase2::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
    
    if (IsValid(RoomSpace))
    {
        RoomSpace->OnRoomEscapeWallDestroyed.RemoveAll(this);
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASLDeveloperBossPhase2::StartPhase()
{
    Super::StartPhase();
    
    UE_LOG(LogTemp, Error, TEXT("Phase2: StartPhase called, RoomSpace valid: %s"), 
           IsValid(RoomSpace) ? TEXT("YES") : TEXT("NO"));
    
    if (!IsValid(RoomSpace))
    {
        UE_LOG(LogTemp, Error, TEXT("Phase2: No RoomSpace, completing immediately"));
        bIsCompleted = true;
        CheckPhaseCompletion();
        return;
    }

    bIsCompleted = false;
    bWaitingForCinematic = false;

    UE_LOG(LogTemp, Error, TEXT("Phase2: Playing start cinematic"));
    PlayCinematic(EPhase2CinematicType::Start);
}

void ASLDeveloperBossPhase2::EndPhase()
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
}

void ASLDeveloperBossPhase2::SetRoomSpace(ASLDeveloperRoomSpace* InRoomSpace)
{
    if (IsValid(RoomSpace))
    {
        RoomSpace->OnRoomEscapeWallDestroyed.RemoveAll(this);
    }
    
    RoomSpace = InRoomSpace;
    
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
}

void ASLDeveloperBossPhase2::OnPhaseEnded()
{
}

void ASLDeveloperBossPhase2::HandleLineDestroyed(int32 LineIndex)
{
    Super::HandleLineDestroyed(LineIndex);
    if (!bIsPhaseActive || bWaitingForCinematic)
    {
        return;
    }
    
    UE_LOG(LogTemp, Error, TEXT("Phase2: Line destroyed, completing phase"));
    
    // 선이 파괴되면 Phase2 완료
    bIsCompleted = true;
    CheckPhaseCompletion();
}

void ASLDeveloperBossPhase2::HandleRoomEscape(ASLDeveloperRoomSpace* Room)
{
    if (!bIsPhaseActive || Room != RoomSpace || bWaitingForCinematic)
    {
        return;
    }
    // 탈출벽 파괴 시 시네마틱 재생
    PlayCinematic(EPhase2CinematicType::Escape);
}

void ASLDeveloperBossPhase2::PlayCinematic(EPhase2CinematicType CinematicType)
{
    CurrentCinematicType = CinematicType;
    
    int32 CinematicIndex = static_cast<int32>(CinematicType);
    
    
    if (!Config.Cinematics.IsValidIndex(CinematicIndex))
    {
        OnCinematicFinished(); // 시네마틱 없으면 바로 완료
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
        CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase2::OnCinematicFinished);
        CurrentSequencePlayer->Play();
       
    }
    else
    {
        OnCinematicFinished();
    }
}

void ASLDeveloperBossPhase2::OnCinematicFinished()
{
    UE_LOG(LogTemp, Error, TEXT("Phase2: OnCinematicFinished - CurrentCinematicType=%d"), 
           static_cast<int32>(CurrentCinematicType));
    bWaitingForCinematic = false;
    
    if (CurrentSequencePlayer)
    {
        CurrentSequencePlayer->OnFinished.RemoveAll(this);
        CurrentSequencePlayer = nullptr;
    }
    
    // 시네마틱 타입에 따른 후속 처리
    switch (CurrentCinematicType)
    {
    case EPhase2CinematicType::Start:
        StartPhaseAfterCinematic();
        break;
        
    case EPhase2CinematicType::Escape:
        // 탈출 시네마틱 완료 후 Phase2 완료
        //bIsCompleted = true;
        //CheckPhaseCompletion();
        break;
    }
}

void ASLDeveloperBossPhase2::StartPhaseAfterCinematic()
{
    
    // 마우스 액터 비활성화
    if (IsValid(MouseActor))
    {
        MouseActor->StopOrbiting();
        MouseActor->SetActorHiddenInGame(true);
        MouseActor->SetActorEnableCollision(false);
    }
    
    // 방 활성화 및 플레이어 텔레포트
    if (IsValid(RoomSpace))
    {
        RoomSpace->ActivateRoom();
        
        if (Config.bAutoTeleportPlayer)
        {
            RoomSpace->TeleportPlayerToRoom();
        }
        
        RoomSpace->SpawnAllNPCs();
    }
}