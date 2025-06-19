#include "SLDeveloperBoss.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLBossCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AI/Actors/SLDeveloperRoomCable.h"
#include "AI/Actors/SLDeveloperRoomSpace.h"
#include "AI/Actors/SLLaunchableWall.h"
#include "AI/Actors/SLMouseActor.h"
#include "Components/CapsuleComponent.h"

ASLDeveloperBoss::ASLDeveloperBoss()
{
    PrimaryActorTick.bCanEverTick = false;

    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    DestroyedLinesCount = 0;
    bIsFightStarted = false;
    bCanLaunchWall = true;
    WallAttackCooldown = 3.0f;
    CurrentWall = nullptr;

    Phase1BossHealthMultiplier = 0.3f;
    Phase1BossSpawnDelay = 2.0f;
    Phase1BossSpawnOffset = FVector(300.0f, 0.0f, 0.0f);

    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;

    bIsPhase2Active = false;

    MouseActor = nullptr;
    MouseActorClass = nullptr;
    bIsPhase3Active = false;
    bIsPhase5Active = false;
    Phase3SurvivalTime = 10.0f;

    Phase3MouseActor = nullptr;
}

void ASLDeveloperBoss::BeginPlay()
{
    Super::BeginPlay();
    
    SetupBossLines();

    if (MouseActorClass)
    {
        SpawnMouseActor();
    }
}

void ASLDeveloperBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    bIsPhase1Active = false;
    bIsPhase3Active = false;
    bIsPhase5Active = false;
    
    if (IsValid(MouseActor))
    {
        MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.RemoveAll(this);
    }
    
    ResetBossLines();
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
        
        UE_LOG(LogTemp, Display, TEXT("Developer Boss spawned with box collision: %s"), *SpawnedBoss->GetName());
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
    
    UE_LOG(LogTemp, Display, TEXT("Developer Boss pattern finished - All bosses despawned"));
}

void ASLDeveloperBoss::InitializeBossFight()
{
    if (bIsFightStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss fight already initialized"));
        return;
    }

    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    DestroyedLinesCount = 0;
    bCanLaunchWall = true;
    
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;
    
    for (int32 i = 0; i < BossLines.Num(); i++)
    {
        if (IsValid(BossLines[i]))
        {
            BossLines[i]->DeactivateLine();
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Developer Boss fight initialized - waiting for dialogue trigger"));
}

void ASLDeveloperBoss::TriggerFirstWallDuringDialogue()
{
    if (bIsFightStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss fight already started"));
        return;
    }

    bIsFightStarted = true;
    
    UE_LOG(LogTemp, Display, TEXT("Dialogue interrupted - First wall attack triggered!"));
    
    LaunchWallAttack();
}

void ASLDeveloperBoss::LaunchWallAttack()
{
    if (!CanLaunchWallAttack() || LaunchableWalls.Num() == 0)
    {
        return;
    }

    ResetCurrentWall();

    ASLLaunchableWall* AvailableWall = nullptr;
    for (ASLLaunchableWall* Wall : LaunchableWalls)
    {
        if (IsValid(Wall) && Wall->CanLaunch())
        {
            AvailableWall = Wall;
            break;
        }
    }

    if (!AvailableWall)
    {
        UE_LOG(LogTemp, Warning, TEXT("No available wall found for attack"));
        return;
    }

    CurrentWall = AvailableWall;
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
    
    UE_LOG(LogTemp, Display, TEXT("Wall attack launched"));
}

EDeveloperBossPhase ASLDeveloperBoss::GetCurrentPhase() const
{
    return CurrentPhase;
}

int32 ASLDeveloperBoss::GetDestroyedLinesCount() const
{
    return DestroyedLinesCount;
}

bool ASLDeveloperBoss::CanLaunchWallAttack() const
{
    return bCanLaunchWall && DestroyedLinesCount < BossLines.Num() && LaunchableWalls.Num() > 0;
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
        
        UE_LOG(LogTemp, Log, TEXT("Dead boss collision completely disabled: %s"), *DeadBoss->GetName());
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
    if (LineIndex < 0 || LineIndex >= BossLines.Num())
    {
        return;
    }

    DestroyedLinesCount++;
    OnBossLineDestroyed.Broadcast(LineIndex);
    
    if (LineIndex == 1 && IsValid(MouseActor))
    {
        MouseActor->StopOrbiting();
    }

    EDeveloperBossPhase NextPhase = static_cast<EDeveloperBossPhase>(static_cast<int32>(CurrentPhase) + 1);
    
    if (DestroyedLinesCount < BossLines.Num())
    {
        ChangePhase(NextPhase);
        StartPhasePattern(NextPhase);
    }
    else
    {
        ChangePhase(EDeveloperBossPhase::Phase5_Final);
        StartPhasePattern(EDeveloperBossPhase::Phase5_Final);
    }
}

void ASLDeveloperBoss::HandleWallAttackFinished()
{
    ActivateNextLine();
    ResetCurrentWall();
    
    UE_LOG(LogTemp, Display, TEXT("Wall attack finished - Line activated"));
}

void ASLDeveloperBoss::RegisterBossEvents(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }
    
    Boss->OnCharacterDeath.AddUObject(this, &ASLDeveloperBoss::HandleBossDeath);
    Boss->OnPatternFinished.AddUObject(this, &ASLDeveloperBoss::HandlePatternFinished);
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

void ASLDeveloperBoss::SetupBossLines()
{
    if (BossLines.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No boss lines assigned in the editor"));
        return;
    }

    for (int32 i = 0; i < BossLines.Num() && i < MaxBossLines; i++)
    {
        if (IsValid(BossLines[i]))
        {
            BossLines[i]->SetLineIndex(i);
            BossLines[i]->OnBossLineDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandleLineDestroyed);
            BossLines[i]->DeactivateLine();
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Boss lines setup complete. Using %d lines"), BossLines.Num());
}

void ASLDeveloperBoss::ActivateNextLine()
{
    if (DestroyedLinesCount >= BossLines.Num())
    {
        return;
    }

    int32 NextLineIndex = DestroyedLinesCount;
    if (BossLines.IsValidIndex(NextLineIndex) && IsValid(BossLines[NextLineIndex]))
    {
        BossLines[NextLineIndex]->ActivateLine();
        UE_LOG(LogTemp, Display, TEXT("Line %d activated"), NextLineIndex);
    }
}

void ASLDeveloperBoss::StartPhasePattern(EDeveloperBossPhase Phase)
{
    switch (Phase)
    {
    case EDeveloperBossPhase::Phase0_Start:
        break;

    case EDeveloperBossPhase::Phase1_BossRush:
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 1: Boss Rush"));
        StartPhase1BossRush();
        break;

    case EDeveloperBossPhase::Phase2_HackSlash:
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 2: Hack & Slash"));
        StartPhase2HackSlash();
        break;

    case EDeveloperBossPhase::Phase3_Horror:
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 3: Horror"));
        StartPhase3Horror();
        break;

    case EDeveloperBossPhase::Phase4_Platformer:
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 4: Platformer"));
        LaunchWallAttack();
        break;

    case EDeveloperBossPhase::Phase5_Final:
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 5: Final - Mouse Actor"));
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
        UE_LOG(LogTemp, Display, TEXT("Phase changed to: %d"), static_cast<int32>(NewPhase));
    }
}

void ASLDeveloperBoss::ResetBossLines()
{
    for (ASLDeveloperRoomCable* Line : BossLines)
    {
        if (IsValid(Line))
        {
            Line->OnBossLineDestroyed.RemoveAll(this);
            Line->DeactivateLine();
        }
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
    
    UE_LOG(LogTemp, Display, TEXT("Test: Killed all bosses"));
}

void ASLDeveloperBoss::TestDestroyNextLine()
{
    if (DestroyedLinesCount >= BossLines.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("All lines already destroyed"));
        return;
    }

    int32 NextLineIndex = DestroyedLinesCount;
    if (BossLines.IsValidIndex(NextLineIndex) && IsValid(BossLines[NextLineIndex]))
    {
        BossLines[NextLineIndex]->DestroyLine();
        UE_LOG(LogTemp, Display, TEXT("Test: Destroyed line %d"), NextLineIndex);
    }
}

void ASLDeveloperBoss::StartPhase1BossRush()
{
    if (AvailableBossClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1: No boss classes available"));
        CompletePhase1BossRush();
        return;
    }

    bIsPhase1Active = true;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = AvailableBossClasses.Num();

    UE_LOG(LogTemp, Display, TEXT("Phase1: Starting Boss Rush with %d bosses"), Phase1TotalBossCount);

    SpawnNextPhase1Boss();
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
        
        UE_LOG(LogTemp, Display, TEXT("Phase1: Spawned boss %d/%d - %s"), 
            Phase1CurrentBossIndex, Phase1TotalBossCount, *SpawnedBoss->GetClass()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1: Failed to spawn boss %d"), Phase1CurrentBossIndex);
        Phase1CurrentBossIndex++;
        
        if (Phase1CurrentBossIndex < AvailableBossClasses.Num())
        {
            FTimerHandle SpawnTimer;
            GetWorldTimerManager().SetTimer(
                SpawnTimer,
                this,
                &ASLDeveloperBoss::OnPhase1SpawnDelayFinished,
                Phase1BossSpawnDelay,
                false
            );
        }
    }
}

void ASLDeveloperBoss::HandlePhase1BossDeath(ASLAIBaseCharacter* DeadBoss)
{
    if (!bIsPhase1Active)
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Phase1: Boss defeated - %s"), *DeadBoss->GetClass()->GetName());

    if (Phase1CurrentBossIndex < AvailableBossClasses.Num())
    {
        UE_LOG(LogTemp, Display, TEXT("Phase1: Spawning next boss in %f seconds"), Phase1BossSpawnDelay);
        
        FTimerHandle SpawnTimer;
        GetWorldTimerManager().SetTimer(
            SpawnTimer,
            this,
            &ASLDeveloperBoss::OnPhase1SpawnDelayFinished,
            Phase1BossSpawnDelay,
            false
        );
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
    
    UE_LOG(LogTemp, Display, TEXT("Phase1: Boss Rush completed! Launching next wall attack"));
    
    OnPhase1BossRushCompleted.Broadcast();
    
    LaunchWallAttack();
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

void ASLDeveloperBoss::StartPhase2HackSlash()
{
   if (!IsValid(Phase2Room))
   {
       UE_LOG(LogTemp, Warning, TEXT("Phase2: No room assigned"));
       LaunchWallAttack();
       return;
   }

   bIsPhase2Active = true;
   
   Phase2Room->OnRoomEscapeWallDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandlePhase2RoomEscape);
   Phase2Room->ActivateRoom();
   
   Phase2Room->TeleportPlayerToRoom();
   Phase2Room->SpawnAllNPCs();
   
   UE_LOG(LogTemp, Display, TEXT("Phase2: Hack & Slash started with teleport"));
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
        
        // 방 비활성화 전에 NPC 정리 강제 실행
        Phase2Room->CleanupNPCs();
        Phase2Room->DeactivateRoom();
    }
    
    UE_LOG(LogTemp, Display, TEXT("Phase2: Player escaped room! Launching next wall attack"));
    
    OnPhase2HackSlashCompleted.Broadcast();
    
    LaunchWallAttack();
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
       
       UE_LOG(LogTemp, Display, TEXT("Mouse Actor destroyed"));
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
   
   UE_LOG(LogTemp, Display, TEXT("Mouse Actor activated"));
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
   
   UE_LOG(LogTemp, Display, TEXT("Mouse Actor deactivated"));
}

ASLMouseActor* ASLDeveloperBoss::GetMouseActor() const
{
   return MouseActor;
}

void ASLDeveloperBoss::HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
   if (DestroyedMouseActor == MouseActor)
   {
       MouseActor = nullptr;
       
       if (bIsPhase5Active)
       {
           bIsPhase5Active = false;
           UE_LOG(LogTemp, Display, TEXT("Phase 5 completed - Mouse Actor destroyed!"));
           
           OnPhase5FinalCompleted.Broadcast();
           OnDeveloperBossPatternFinished.Broadcast();
       }
   }
}

void ASLDeveloperBoss::StartPhase3Horror()
{
    if (!IsValid(MouseActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: No mouse actor available"));
        LaunchWallAttack();
        return;
    }
    
    bIsPhase3Active = true;
    
    // 기존 마우스 액터 비활성화
    DeactivateMouseActor();
    
    // Phase3 전용 마우스 액터 스폰
    SpawnPhase3MouseActor();
    
    UE_LOG(LogTemp, Display, TEXT("Phase3: Horror phase started - survive for %f seconds"), Phase3SurvivalTime);
    
    FTimerHandle SurvivalTimer;
    GetWorldTimerManager().SetTimer(
        SurvivalTimer,
        this,
        &ASLDeveloperBoss::OnPhase3SurvivalTimeFinished,
        Phase3SurvivalTime,
        false
    );
}

void ASLDeveloperBoss::StartPhase5Final()
{
   if (!IsValid(MouseActor))
   {
       OnDeveloperBossPatternFinished.Broadcast();
       return;
   }
   
   bIsPhase5Active = true;
   
   ActivateMouseActor();
   
   UE_LOG(LogTemp, Display, TEXT("Phase5: Final phase started - destroy the mouse actor to win!"));
}

void ASLDeveloperBoss::TestStartPhase3()
{
   CurrentPhase = EDeveloperBossPhase::Phase3_Horror;
   StartPhase3Horror();
   UE_LOG(LogTemp, Display, TEXT("Test: Started Phase3 Horror"));
}

void ASLDeveloperBoss::TestStartPhase5()
{
   CurrentPhase = EDeveloperBossPhase::Phase5_Final;
   StartPhase5Final();
   UE_LOG(LogTemp, Display, TEXT("Test: Started Phase5 Final"));
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
   
   UE_LOG(LogTemp, Log, TEXT("Scheduled %d dead bosses for cleanup"), BossesToRemove.Num());
}

void ASLDeveloperBoss::OnWallCooldownFinishedInternal()
{
   bCanLaunchWall = true;
   OnWallCooldownFinished.Broadcast();
}

void ASLDeveloperBoss::OnPhase1SpawnDelayFinished()
{
   SpawnNextPhase1Boss();
}

void ASLDeveloperBoss::OnPhase3SurvivalTimeFinished()
{
    if (bIsPhase3Active)
    {
        bIsPhase3Active = false;
        
        // Phase3 마우스 액터 제거
        DestroyPhase3MouseActor();
        
        // 기존 마우스 액터 다시 활성화
        ActivateMouseActor();
        
        UE_LOG(LogTemp, Display, TEXT("Phase3: Survival time completed! Launching next wall"));
        
        OnPhase3HorrorCompleted.Broadcast();
        LaunchWallAttack();
    }
}

void ASLDeveloperBoss::SpawnPhase3MouseActor()
{
    // 기존 Phase3 마우스 액터가 있다면 제거
    if (IsValid(Phase3MouseActor))
    {
        DestroyPhase3MouseActor();
    }
    
    // MouseActor에서 Phase3MouseActorClass 가져오기
    TSubclassOf<ASLMouseActor> Phase3Class = nullptr;
    if (IsValid(MouseActor))
    {
        Phase3Class = MouseActor->GetPhase3MouseActorClass();
    }
    
    if (!Phase3Class)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase3: No Phase3 Mouse Actor class specified"));
        return;
    }
    
    // 기존 마우스 액터 위치에서 스폰
    FVector SpawnLocation = MouseActor ? MouseActor->GetActorLocation() : GetActorLocation() + FVector(0, 0, 500.0f);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    Phase3MouseActor = GetWorld()->SpawnActor<ASLMouseActor>(Phase3Class, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.AddDynamic(this, &ASLDeveloperBoss::HandlePhase3MouseActorDestroyed);
        
        // Phase3 모드 즉시 시작
        Phase3MouseActor->StartPhase3HorrorMode();
        
        UE_LOG(LogTemp, Display, TEXT("Phase3 Mouse Actor spawned successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn Phase3 Mouse Actor"));
    }
}

void ASLDeveloperBoss::DestroyPhase3MouseActor()
{
    if (IsValid(Phase3MouseActor))
    {
        Phase3MouseActor->OnMouseActorDestroyed.RemoveAll(this);
        Phase3MouseActor->Destroy();
        Phase3MouseActor = nullptr;
        
        UE_LOG(LogTemp, Display, TEXT("Phase3 Mouse Actor destroyed"));
    }
}

void ASLDeveloperBoss::HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor)
{
    if (DestroyedMouseActor == Phase3MouseActor)
    {
        Phase3MouseActor = nullptr;
        
        if (bIsPhase3Active)
        {
            // Phase3에서 Phase3 마우스 액터가 파괴되면 즉시 Phase3 종료
            bIsPhase3Active = false;
            OnPhase3HorrorCompleted.Broadcast();
            LaunchWallAttack();
            
            UE_LOG(LogTemp, Display, TEXT("Phase3 completed - Phase3 Mouse Actor destroyed!"));
        }
    }
}