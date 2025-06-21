#include "SLDeveloperRoomSpace.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
#include "Interactable/Object/SLInteractableBreakable.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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

    EscapeWallChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("EscapeWallChildActorComponent"));
    EscapeWallChildActorComponent->SetupAttachment(RootComponent);

    PlayerTeleportLocation = FVector::ZeroVector;
    EscapeWallActor = nullptr;
    bIsRoomActive = false;
    bShowEscapeWallOnStart = true;
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
    if (EscapeWallChildActorComponent)
    {
        AActor* ChildActor = EscapeWallChildActorComponent->GetChildActor();
        EscapeWallActor = Cast<ASLInteractableBreakable>(ChildActor);

        if (IsValid(EscapeWallActor))
        {
            EscapeWallActor->OnObjectBreaked.AddDynamic(this, &ASLDeveloperRoomSpace::OnEscapeWallDestroyed);
            
            if (bShowEscapeWallOnStart)
            {
                EscapeWallActor->SetActorHiddenInGame(false);
                EscapeWallActor->SetActorEnableCollision(true);
                EscapeWallActor->SetActorTickEnabled(true);
            }
            else
            {
                EscapeWallActor->SetActorHiddenInGame(true);
                EscapeWallActor->SetActorEnableCollision(false);
                EscapeWallActor->SetActorTickEnabled(false);
            }
        }
    }
}

void ASLDeveloperRoomSpace::ActivateRoom()
{
    bIsRoomActive = true;
    
    if (RoomMeshComponent)
    {
        RoomMeshComponent->SetVisibility(true);
        RoomMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (IsValid(EscapeWallActor))
    {
        EscapeWallActor->SetActorHiddenInGame(false);
        EscapeWallActor->SetActorEnableCollision(true);
        EscapeWallActor->SetActorTickEnabled(true);
    }
}

void ASLDeveloperRoomSpace::DeactivateRoom()
{
    bIsRoomActive = false;
    CleanupNPCs();

    if (IsValid(EscapeWallActor))
    {
        EscapeWallActor->SetActorHiddenInGame(true);
        EscapeWallActor->SetActorEnableCollision(false);
        EscapeWallActor->SetActorTickEnabled(false);
    }
}

void ASLDeveloperRoomSpace::OnEscapeWallDestroyed()
{
    RemoveAllNPCWeapons();
    
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
}

void ASLDeveloperRoomSpace::TeleportPlayerToRoom()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            FVector TeleportLocation = GetActorLocation() + PlayerTeleportLocation;
            PlayerPawn->SetActorLocation(TeleportLocation);
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
        OnPlayerEnteredRoom.Broadcast(this);
    }
}

void ASLDeveloperRoomSpace::OnNPCDeath(ASLAIBaseCharacter* DeadNPC)
{
    if (SpawnedNPCs.Contains(DeadNPC))
    {
        // NPC가 장착한 무기 정리
        CleanupNPCEquipment(DeadNPC);
        
        SpawnedNPCs.Remove(DeadNPC);
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
            
            // NPC 장비 정리
            CleanupNPCEquipment(NPC);
            
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

void ASLDeveloperRoomSpace::RemoveAllNPCWeapons()
{
    for (ASLAIBaseCharacter* NPC : SpawnedNPCs)
    {
        if (IsValid(NPC))
        {
            NPC->UnequipWeapon();
        }
    }
    
}

void ASLDeveloperRoomSpace::CleanupNPCEquipment(ASLAIBaseCharacter* NPC)
{
    if (!IsValid(NPC))
    {
        return;
    }
    
    if (AActor* EquippedWeapon = NPC->GetEquippedWeapon())
    {
        NPC->UnequipWeapon();
        if (IsValid(EquippedWeapon))
        {
            EquippedWeapon->Destroy();
        }
    }
}