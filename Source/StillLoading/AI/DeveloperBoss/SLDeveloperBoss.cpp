#include "SLDeveloperBoss.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLBossCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AI/Actors/SLDeveloperBossLine.h"
#include "AI/Actors/SLLaunchableWall.h"

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
}

void ASLDeveloperBoss::BeginPlay()
{
    Super::BeginPlay();
    
    SetupBossLines();
}

void ASLDeveloperBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 타이머들 정리
    GetWorldTimerManager().ClearTimer(WallCooldownTimer);
    GetWorldTimerManager().ClearTimer(Phase1SpawnTimer);
    
    // Phase1 상태 정리
    bIsPhase1Active = false;
    
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
        
        UE_LOG(LogTemp, Display, TEXT("Developer Boss spawned: %s"), *SpawnedBoss->GetName());
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
    
    // Phase1 상태 초기화
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;
    GetWorldTimerManager().ClearTimer(Phase1SpawnTimer);
    
    // 모든 선을 비활성 상태로 초기화
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
    
    // 대화 중 첫 번째 벽 발사
    LaunchWallAttack();
}

void ASLDeveloperBoss::LaunchWallAttack()
{
    if (!CanLaunchWallAttack() || LaunchableWalls.Num() == 0)
    {
        return;
    }

    ResetCurrentWall();

    // 사용 가능한 벽 찾기
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
    GetWorldTimerManager().SetTimer(
        WallCooldownTimer,
        [this]() { bCanLaunchWall = true; },
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

        // Phase1일 때는 별도 처리
        if (bIsPhase1Active)
        {
            HandlePhase1BossDeath(DeadBoss);
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
    if (LineIndex < 0 || LineIndex >= BossLines.Num())
    {
        return;
    }

    DestroyedLinesCount++;
    OnBossLineDestroyed.Broadcast(LineIndex);
    
    UE_LOG(LogTemp, Display, TEXT("Line %d destroyed. Total destroyed: %d"), LineIndex, DestroyedLinesCount);

    // 페이즈 전환만 수행 (벽 발사는 각 페이즈에서 별도 처리)
    EDeveloperBossPhase NextPhase = static_cast<EDeveloperBossPhase>(static_cast<int32>(CurrentPhase) + 1);
    
    if (DestroyedLinesCount < BossLines.Num())
    {
        ChangePhase(NextPhase);
        StartPhasePattern(NextPhase);
    }
    else
    {
        // 모든 선이 파괴됨 - 최종 페이즈
        ChangePhase(EDeveloperBossPhase::Phase5_Final);
        StartPhasePattern(EDeveloperBossPhase::Phase5_Final);
    }
}

void ASLDeveloperBoss::HandleWallAttackFinished()
{
    // 벽 공격이 완료되면 해당 벽 뒤의 선을 활성화
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
            BossLines[i]->DeactivateLine(); // 초기 상태는 비활성
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
        // 시작 페이즈 - 특별한 패턴 없음
        // 첫 번째 선이 파괴되면 자동으로 Phase1으로 전환됨
        break;

    case EDeveloperBossPhase::Phase1_BossRush:
        // 추억의 보스 러시 패턴
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 1: Boss Rush"));
        StartPhase1BossRush();
        break;

    case EDeveloperBossPhase::Phase2_HackSlash:
        // 핵앤슬래시 패턴
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 2: Hack & Slash"));
        // TODO: 특정 공간으로 이동
        // TODO: NPC들 스폰하여 플레이어 추격
        // TODO: 맵 끝의 벽을 부수면 LaunchWallAttack() 호출
        break;

    case EDeveloperBossPhase::Phase3_Horror:
        // 공포 게임 패턴
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 3: Horror"));
        // TODO: 어두워지고 마우스 무서운 형상 컷신
        // TODO: 컷신 끝날 때 N초 후 LaunchWallAttack() 예약
        // TODO: 마우스가 빠르게 추격, N초 버티면 선이 드러남
        break;

    case EDeveloperBossPhase::Phase4_Platformer:
        // 점프맵 패턴
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 4: Platformer"));
        // TODO: 땅 파괴하고 발판 소환
        // TODO: 발판이 전부 원래 자리까지 올라가면 LaunchWallAttack() 호출
        break;

    case EDeveloperBossPhase::Phase5_Final:
        // 최종 페이즈 - 마우스 액터
        UE_LOG(LogTemp, Display, TEXT("Starting Phase 5: Final - Mouse Actor"));
        // TODO: 마우스를 공격할 수 있게 됨
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
    for (ASLDeveloperBossLine* Line : BossLines)
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

void ASLDeveloperBoss::TestSpawnRandomBoss()
{
    if (AvailableBossClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No boss classes available to spawn"));
        return;
    }
    
    int32 RandomIndex = FMath::RandRange(0, AvailableBossClasses.Num() - 1);
    TSubclassOf<ASLAIBaseCharacter> BossClass = AvailableBossClasses[RandomIndex];
    
    FVector SpawnLocation = GetActorLocation() + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 100.f);
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
    
    ASLAIBaseCharacter* SpawnedBoss = SpawnBossCharacter(BossClass, SpawnTransform);
    
    if (SpawnedBoss)
    {
        UE_LOG(LogTemp, Display, TEXT("Test: Spawned %s at %s"), 
            *SpawnedBoss->GetClass()->GetName(), 
            *SpawnLocation.ToString());
    }
}

void ASLDeveloperBoss::TestSpawnAllBosses()
{
    float Radius = 500.f;
    int32 BossCount = AvailableBossClasses.Num();
    
    for (int32 i = 0; i < BossCount; i++)
    {
        float Angle = (360.f / BossCount) * i;
        float RadAngle = FMath::DegreesToRadians(Angle);
        
        FVector SpawnLocation = GetActorLocation() + FVector(
            FMath::Cos(RadAngle) * Radius,
            FMath::Sin(RadAngle) * Radius,
            100.f
        );
        
        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
        SpawnBossCharacter(AvailableBossClasses[i], SpawnTransform);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Test: Spawned all %d bosses"), BossCount);
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

/**
 * @brief 다음 파괴 대상 라인을 제거하고 로그에 정보를 출력합니다.
 *
 * 해당 메소드는 사전에 설정된 BossLines 배열에서 DestroyedLinesCount의 값에 해당하는 라인을 파괴합니다.
 * 만약 DestroyedLinesCount가 BossLines 배열의 크기와 같거나 이를 초과하면, 모든 라인이 이미 파괴되었다고 로그를 남기고 아무 작업도 수행하지 않습니다.
 *
 * @details
 * 1. 현재 DestroyedLinesCount 값이 BossLines 배열 범위를 초과했는지 확인합니다.
 *    - 초과할 경우 로그를 출력하고 메서드를 종료합니다.
 * 2. BossLines 배열에서 DestroyedLinesCount 인덱스에 해당하는 라인이 유효한지 검사합니다.
 *    - 유효하다면 해당 라인을 DestroyLine() 메서드를 호출하여 파괴합니다.
 *    - 파괴된 라인의 인덱스를 로그에 출력합니다.
 *
 * @warning BossLines 배열 또는 각 요소가 올바르게 초기화되어 있지 않을 경우 정상 동작을 기대할 수 없습니다.
 */
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

void ASLDeveloperBoss::TestResetAllLines()
{
    DestroyedLinesCount = 0;
    CurrentPhase = EDeveloperBossPhase::Phase0_Start;
    bIsFightStarted = false; // 전투 시작 상태도 리셋
    
    // Phase1 상태 리셋
    bIsPhase1Active = false;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = 0;
    GetWorldTimerManager().ClearTimer(Phase1SpawnTimer);
    
    for (int32 i = 0; i < BossLines.Num(); i++)
    {
        if (IsValid(BossLines[i]))
        {
            BossLines[i]->DeactivateLine();
        }
    }
    
    // 소환된 보스들도 정리
    DespawnAllBosses();
    
    UE_LOG(LogTemp, Display, TEXT("Test: Reset all lines - Call InitializeBossFight() to prepare"));
}

void ASLDeveloperBoss::TestTriggerDialogueWall()
{
    if (!bIsFightStarted)
    {
        TriggerFirstWallDuringDialogue();
        UE_LOG(LogTemp, Display, TEXT("Test: Triggered dialogue wall attack"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Test: Fight already started"));
    }
}

void ASLDeveloperBoss::TestStartPhase1()
{
    if (bIsPhase1Active)
    {
        UE_LOG(LogTemp, Warning, TEXT("Test: Phase1 already active"));
        return;
    }

    CurrentPhase = EDeveloperBossPhase::Phase1_BossRush;
    StartPhase1BossRush();
    UE_LOG(LogTemp, Display, TEXT("Test: Started Phase1 Boss Rush"));
}

void ASLDeveloperBoss::StartPhase1BossRush()
{
    if (AvailableBossClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1: No boss classes available"));
        CompletePhase1BossRush(); // 보스가 없으면 바로 완료
        return;
    }

    bIsPhase1Active = true;
    Phase1CurrentBossIndex = 0;
    Phase1TotalBossCount = AvailableBossClasses.Num();

    UE_LOG(LogTemp, Display, TEXT("Phase1: Starting Boss Rush with %d bosses"), Phase1TotalBossCount);

    // 첫 번째 보스 즉시 소환
    SpawnNextPhase1Boss();
}

void ASLDeveloperBoss::SpawnNextPhase1Boss()
{
    if (!bIsPhase1Active || Phase1CurrentBossIndex >= AvailableBossClasses.Num())
    {
        return;
    }

    TSubclassOf<ASLAIBaseCharacter> BossClass = AvailableBossClasses[Phase1CurrentBossIndex];
    
    // 스폰 위치 계산 (개발자 보스 주변)
    FVector SpawnLocation = GetActorLocation() + Phase1BossSpawnOffset;
    SpawnLocation += FVector(FMath::RandRange(-100.f, 100.f), FMath::RandRange(-100.f, 100.f), 0.f);
    
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
    
    ASLAIBaseCharacter* SpawnedBoss = SpawnBossCharacter(BossClass, SpawnTransform);
    
    if (SpawnedBoss)
    {
        // 보스 약화 처리
        WeakenBossForPhase1(SpawnedBoss);
        
        Phase1CurrentBossIndex++;
        
        UE_LOG(LogTemp, Display, TEXT("Phase1: Spawned boss %d/%d - %s"), 
            Phase1CurrentBossIndex, Phase1TotalBossCount, *SpawnedBoss->GetClass()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Phase1: Failed to spawn boss %d"), Phase1CurrentBossIndex);
        Phase1CurrentBossIndex++; // 실패해도 다음으로 진행
        
        // 다음 보스 소환 시도
        if (Phase1CurrentBossIndex < AvailableBossClasses.Num())
        {
            GetWorldTimerManager().SetTimer(
                Phase1SpawnTimer,
                this,
                &ASLDeveloperBoss::SpawnNextPhase1Boss,
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

    // 다음 보스가 있으면 딜레이 후 소환
    if (Phase1CurrentBossIndex < AvailableBossClasses.Num())
    {
        UE_LOG(LogTemp, Display, TEXT("Phase1: Spawning next boss in %f seconds"), Phase1BossSpawnDelay);
        
        GetWorldTimerManager().SetTimer(
            Phase1SpawnTimer,
            this,
            &ASLDeveloperBoss::SpawnNextPhase1Boss,
            Phase1BossSpawnDelay,
            false
        );
    }
    else
    {
        // 모든 보스를 소환했고, 현재 살아있는 보스가 없으면 Phase1 완료
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
    
    // 타이머 정리
    GetWorldTimerManager().ClearTimer(Phase1SpawnTimer);
    
    UE_LOG(LogTemp, Display, TEXT("Phase1: Boss Rush completed! Launching next wall attack"));
    
    // 다음 벽 공격 발사
    LaunchWallAttack();
}

void ASLDeveloperBoss::WeakenBossForPhase1(ASLAIBaseCharacter* Boss)
{
    if (!Boss)
    {
        return;
    }

    // 보스 체력 감소 (30%로 설정)
    float OriginalMaxHealth = Boss->GetMaxHealth();
    float WeakenedHealth = OriginalMaxHealth * Phase1BossHealthMultiplier;
    
    Boss->SetMaxHealth(WeakenedHealth);
    Boss->SetCurrentHealth(WeakenedHealth);
    
    // 특수 패턴 비활성화 (보스 러시용)
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