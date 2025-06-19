#include "SLDeveloperRoomSpace.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ASLDeveloperRoomSpace::ASLDeveloperRoomSpace()
{
    PrimaryActorTick.bCanEverTick = false;

    RoomMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomMeshComponent"));
    RootComponent = RoomMeshComponent;

    PlayerDetectionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDetectionComponent"));
    PlayerDetectionComponent->SetupAttachment(RootComponent);

    NPCSpawnAreaComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("NPCSpawnAreaComponent"));
    NPCSpawnAreaComponent->SetupAttachment(RootComponent);
    NPCSpawnAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    NPCSpawnAreaComponent->SetVisibility(true);

    // Escape Wall 컴포넌트들 생성
    EscapeWallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EscapeWallMeshComponent"));
    EscapeWallMeshComponent->SetupAttachment(RootComponent);

    EscapeWallCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("EscapeWallCollisionComponent"));
    EscapeWallCollisionComponent->SetupAttachment(EscapeWallMeshComponent);
    EscapeWallCollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

    PlayerTeleportLocation = FVector::ZeroVector;
    EscapeWallLocation = FVector(500.0f, 0.0f, 0.0f);
    EscapeWallRotation = FRotator::ZeroRotator;
    EscapeWallScale = FVector(1.0f, 1.0f, 1.0f);
    EscapeWallMaxHp = 10;
    EscapeWallCurrentHp = EscapeWallMaxHp;
    bIsRoomActive = false;
    bEscapeWallDestroyed = false;
}

void ASLDeveloperRoomSpace::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerDetectionComponent)
    {
        PlayerDetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLDeveloperRoomSpace::OnPlayerOverlapBegin);
    }

    SetupEscapeWall();
}

void ASLDeveloperRoomSpace::SetupEscapeWall()
{
    if (EscapeWallMeshComponent)
    {
        // 메시 설정
        if (EscapeWallMesh)
        {
            EscapeWallMeshComponent->SetStaticMesh(EscapeWallMesh);
        }

        // 위치, 회전, 스케일 설정
        EscapeWallMeshComponent->SetRelativeLocation(EscapeWallLocation);
        EscapeWallMeshComponent->SetRelativeRotation(EscapeWallRotation);
        EscapeWallMeshComponent->SetRelativeScale3D(EscapeWallScale);

        // 초기에는 숨김
        EscapeWallMeshComponent->SetVisibility(false);
        EscapeWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (EscapeWallCollisionComponent)
    {
        EscapeWallCollisionComponent->OnComponentHit.AddDynamic(this, &ASLDeveloperRoomSpace::OnEscapeWallHit);
        EscapeWallCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // HP 초기화
    EscapeWallCurrentHp = EscapeWallMaxHp;
    bEscapeWallDestroyed = false;
}

void ASLDeveloperRoomSpace::ActivateRoom()
{
    bIsRoomActive = true;
    
    if (RoomMeshComponent)
    {
        RoomMeshComponent->SetVisibility(true);
        RoomMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    // Escape Wall 활성화
    if (EscapeWallMeshComponent)
    {
        EscapeWallMeshComponent->SetVisibility(true);
        EscapeWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (EscapeWallCollisionComponent)
    {
        EscapeWallCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    // HP 리셋
    EscapeWallCurrentHp = EscapeWallMaxHp;
    bEscapeWallDestroyed = false;

    UE_LOG(LogTemp, Log, TEXT("Developer Room activated with escape wall"));
}

void ASLDeveloperRoomSpace::DeactivateRoom()
{
    bIsRoomActive = false;
    CleanupNPCs();

    // Escape Wall 비활성화
    if (EscapeWallMeshComponent)
    {
        EscapeWallMeshComponent->SetVisibility(false);
        EscapeWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (EscapeWallCollisionComponent)
    {
        EscapeWallCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    UE_LOG(LogTemp, Log, TEXT("Developer Room deactivated"));
}

void ASLDeveloperRoomSpace::OnEscapeWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bEscapeWallDestroyed || !bIsRoomActive)
    {
        return;
    }

    // 플레이어의 공격인지 확인
    if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
    {
        EscapeWallCurrentHp--;
        UE_LOG(LogTemp, Log, TEXT("Escape wall hit! HP remaining: %d"), EscapeWallCurrentHp);

        if (EscapeWallCurrentHp <= 0)
        {
            DestroyEscapeWall();
        }
    }
    // 또는 BattleComponent를 통한 공격 감지도 가능
    else if (UBattleComponent* BattleComp = OtherActor->FindComponentByClass<UBattleComponent>())
    {
        EscapeWallCurrentHp--;
        UE_LOG(LogTemp, Log, TEXT("Escape wall hit by battle component! HP remaining: %d"), EscapeWallCurrentHp);

        if (EscapeWallCurrentHp <= 0)
        {
            DestroyEscapeWall();
        }
    }
}

void ASLDeveloperRoomSpace::DestroyEscapeWall()
{
    if (bEscapeWallDestroyed)
    {
        return;
    }

    bEscapeWallDestroyed = true;

    // 파괴 이펙트 재생
    if (EscapeWallDestroyEffect && EscapeWallMeshComponent)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            EscapeWallDestroyEffect,
            EscapeWallMeshComponent->GetComponentLocation(),
            EscapeWallMeshComponent->GetComponentRotation()
        );
    }

    // 벽 숨기기
    if (EscapeWallMeshComponent)
    {
        EscapeWallMeshComponent->SetVisibility(false);
        EscapeWallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (EscapeWallCollisionComponent)
    {
        EscapeWallCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    UE_LOG(LogTemp, Log, TEXT("Escape wall destroyed - broadcasting room escape"));
    OnRoomEscapeWallDestroyed.Broadcast(this);
}

void ASLDeveloperRoomSpace::SpawnAllNPCs()
{
    if (NPCSpawnInfos.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No NPC spawn infos configured"));
        return;
    }

    CleanupNPCs();

    for (const FPhase2NPCSpawnInfo& SpawnInfo : NPCSpawnInfos)
    {
        SpawnNPCsFromInfo(SpawnInfo);
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned total %d NPCs in room"), SpawnedNPCs.Num());
}

void ASLDeveloperRoomSpace::TeleportPlayerToRoom()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            FVector TeleportLocation = GetActorLocation() + PlayerTeleportLocation;
            PlayerPawn->SetActorLocation(TeleportLocation);
            UE_LOG(LogTemp, Log, TEXT("Player teleported to room at %s"), *TeleportLocation.ToString());
        }
    }
}

bool ASLDeveloperRoomSpace::IsPlayerInRoom() const
{
    if (!PlayerDetectionComponent)
    {
        return false;
    }

    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            return PlayerDetectionComponent->IsOverlappingActor(PlayerPawn);
        }
    }

    return false;
}

void ASLDeveloperRoomSpace::OnPlayerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Log, TEXT("Player entered room"));
        OnPlayerEnteredRoom.Broadcast(this);
    }
}

void ASLDeveloperRoomSpace::OnNPCDeath(ASLAIBaseCharacter* DeadNPC)
{
    if (SpawnedNPCs.Contains(DeadNPC))
    {
        SpawnedNPCs.Remove(DeadNPC);
        UE_LOG(LogTemp, Log, TEXT("NPC died. Remaining: %d"), SpawnedNPCs.Num());
    }
}

void ASLDeveloperRoomSpace::SpawnNPCsFromInfo(const FPhase2NPCSpawnInfo& SpawnInfo)
{
    if (!SpawnInfo.NPCClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("No NPC class specified in spawn info"));
        return;
    }

    for (int32 i = 0; i < SpawnInfo.SpawnCount; i++)
    {
        FVector SpawnLocation = GetRandomLocationInSpawnArea();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ASLAIBaseCharacter* SpawnedNPC = GetWorld()->SpawnActor<ASLAIBaseCharacter>(
            SpawnInfo.NPCClass,
            SpawnLocation,
            GetActorRotation(),
            SpawnParams
        );

        if (SpawnedNPC)
        {
            SpawnedNPC->OnCharacterDeath.AddUObject(this, &ASLDeveloperRoomSpace::OnNPCDeath);
            SpawnedNPCs.Add(SpawnedNPC);

            UE_LOG(LogTemp, Log, TEXT("Spawned NPC: %s at %s"), 
                *SpawnedNPC->GetClass()->GetName(), 
                *SpawnLocation.ToString());
        }
    }
}

void ASLDeveloperRoomSpace::CleanupNPCs()
{
    for (ASLAIBaseCharacter* NPC : SpawnedNPCs)
    {
        if (IsValid(NPC))
        {
            NPC->OnCharacterDeath.RemoveAll(this);
            NPC->Destroy();
        }
    }
    SpawnedNPCs.Empty();
}

FVector ASLDeveloperRoomSpace::GetRandomLocationInSpawnArea()
{
    if (!NPCSpawnAreaComponent)
    {
        return GetActorLocation();
    }

    FVector BoxExtent = NPCSpawnAreaComponent->GetScaledBoxExtent();
    FVector BoxCenter = NPCSpawnAreaComponent->GetComponentLocation();

    FVector RandomOffset = FVector(
        FMath::RandRange(-BoxExtent.X, BoxExtent.X),
        FMath::RandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::RandRange(-BoxExtent.Z, BoxExtent.Z)
    );

    return BoxCenter + RandomOffset;
}