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
    
    if (!IsValid(RoomSpace))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase2: No room space configured"));
        bIsCompleted = true;
        CheckPhaseCompletion();
        return;
    }

    bIsCompleted = false;
    bWaitingForCinematic = false;

    UE_LOG(LogTemp, Warning, TEXT("🎬 Phase2: Starting with start cinematic"));
    
    // Phase2 시작 시네마틱 재생
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
    
    UE_LOG(LogTemp, Warning, TEXT("📋 Phase2 Config Set:"));
    UE_LOG(LogTemp, Warning, TEXT("  - RoomActivationDelay: %f"), Config.RoomActivationDelay);
    UE_LOG(LogTemp, Warning, TEXT("  - bAutoTeleportPlayer: %s"), Config.bAutoTeleportPlayer ? TEXT("true") : TEXT("false"));
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
    UE_LOG(LogTemp, Display, TEXT("Phase 2 Hack & Slash Started"));
}

void ASLDeveloperBossPhase2::OnPhaseEnded()
{
    UE_LOG(LogTemp, Display, TEXT("Phase 2 Hack & Slash Completed"));
}

void ASLDeveloperBossPhase2::HandleRoomEscape(ASLDeveloperRoomSpace* Room)
{
    if (!bIsPhaseActive || Room != RoomSpace || bWaitingForCinematic)
    {
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🚪 Phase2: Room escape detected"));
    
    // 탈출벽 파괴 시 시네마틱 재생
    PlayCinematic(EPhase2CinematicType::Escape);
}

void ASLDeveloperBossPhase2::PlayCinematic(EPhase2CinematicType CinematicType)
{
    CurrentCinematicType = CinematicType;
    
    int32 CinematicIndex = static_cast<int32>(CinematicType);
    
    UE_LOG(LogTemp, Warning, TEXT("🎭 Phase2: Playing cinematic type: %d, index: %d"), 
           static_cast<int32>(CinematicType), CinematicIndex);
    
    if (!Config.Cinematics.IsValidIndex(CinematicIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Phase2: Invalid CinematicIndex: %d"), CinematicIndex);
        OnCinematicFinished(); // 시네마틱 없으면 바로 완료
        return;
    }
    
    if (!IsValid(Config.Cinematics[CinematicIndex]))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Phase2: Cinematic is null at index: %d"), CinematicIndex);
        OnCinematicFinished();
        return;
    }
    
    UE_LOG(LogTemp, Display, TEXT("✅ Phase2: Starting cinematic: %s"), *Config.Cinematics[CinematicIndex]->GetName());
    
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
        UE_LOG(LogTemp, Display, TEXT("🎮 Phase2: SequencePlayer created successfully"));
        CurrentSequencePlayer->OnFinished.AddDynamic(this, &ASLDeveloperBossPhase2::OnCinematicFinished);
        CurrentSequencePlayer->Play();
        UE_LOG(LogTemp, Display, TEXT("▶️ Phase2: Cinematic play started"));
        
        // 강제 타임아웃 추가 (시네마틱이 멈췄을 경우 대비)
        if (IsValid(GetWorld()))
        {
            GetWorld()->GetTimerManager().SetTimer(
                CinematicTimeoutTimer,
                [this]()
                {
                    if (bWaitingForCinematic)
                    {
                        UE_LOG(LogTemp, Error, TEXT("⏰ Phase2: Cinematic timeout! Force finishing..."));
                        OnCinematicFinished();
                    }
                },
                10.0f, // 10초 타임아웃
                false
            );
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Phase2: Failed to create SequencePlayer"));
        OnCinematicFinished();
    }
}

void ASLDeveloperBossPhase2::OnCinematicFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("🎬 Phase2: OnCinematicFinished called for type: %d"), 
           static_cast<int32>(CurrentCinematicType));
    
    bWaitingForCinematic = false;
    
    // 타임아웃 타이머 클리어
    if (IsValid(GetWorld()) && CinematicTimeoutTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(CinematicTimeoutTimer);
    }
    
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
        bIsCompleted = true;
        CheckPhaseCompletion();
        break;
    }
}

void ASLDeveloperBossPhase2::StartPhaseAfterCinematic()
{
    UE_LOG(LogTemp, Warning, TEXT("🎮 Phase2: Starting gameplay after start cinematic"));
    
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