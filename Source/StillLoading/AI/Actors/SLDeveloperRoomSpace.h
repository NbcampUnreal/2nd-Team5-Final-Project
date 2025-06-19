#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SLDeveloperRoomSpace.generated.h"

class UNiagaraSystem;
class ASLAIBaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnRoomEscapeWallDestroyed, ASLDeveloperRoomSpace*, Room);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnPlayerEnteredRoom, ASLDeveloperRoomSpace*, Room);

USTRUCT(BlueprintType)
struct FPhase2NPCSpawnInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawn")
    TSubclassOf<ASLAIBaseCharacter> NPCClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Spawn")
    int32 SpawnCount;

    FPhase2NPCSpawnInfo()
    {
        NPCClass = nullptr;
        SpawnCount = 1;
    }
};

UCLASS()
class STILLLOADING_API ASLDeveloperRoomSpace : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperRoomSpace();

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    void ActivateRoom();

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    void DeactivateRoom();

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    void SpawnAllNPCs();

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    void TeleportPlayerToRoom();

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    bool IsPlayerInRoom() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Room")
    void DestroyEscapeWall();

    UPROPERTY(BlueprintAssignable, Category = "Developer Room")
    FSLOnRoomEscapeWallDestroyed OnRoomEscapeWallDestroyed;

    UPROPERTY(BlueprintAssignable, Category = "Developer Room")
    FSLOnPlayerEnteredRoom OnPlayerEnteredRoom;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPlayerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnNPCDeath(ASLAIBaseCharacter* DeadNPC);

    UFUNCTION()
    void OnEscapeWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    void SpawnNPCsFromInfo(const FPhase2NPCSpawnInfo& SpawnInfo);
    void CleanupNPCs();
    FVector GetRandomLocationInSpawnArea();
    void SetupEscapeWall();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> RoomMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> PlayerDetectionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> NPCSpawnAreaComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> EscapeWallMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> EscapeWallCollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|NPCs")
    TArray<FPhase2NPCSpawnInfo> NPCSpawnInfos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room")
    FVector PlayerTeleportLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    TObjectPtr<UStaticMesh> EscapeWallMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    FVector EscapeWallLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    FRotator EscapeWallRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    FVector EscapeWallScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    int32 EscapeWallMaxHp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Room|Escape Wall")
    TObjectPtr<UNiagaraSystem> EscapeWallDestroyEffect;

private:
    TArray<TObjectPtr<ASLAIBaseCharacter>> SpawnedNPCs;
    bool bIsRoomActive;
    int32 EscapeWallCurrentHp;
    bool bEscapeWallDestroyed;
};