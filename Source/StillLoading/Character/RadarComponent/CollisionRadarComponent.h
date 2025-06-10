#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "CollisionRadarComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCollisionRadarComponent, Log, All);

USTRUCT(Atomic, BlueprintType)
struct FDetectedActorInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Radar")
    AActor* Actor = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Radar")
    float Distance = 0.0f;
};

USTRUCT(Atomic, BlueprintType)
struct FDetectedActorList
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Radar")
    TArray<FDetectedActorInfo> DetectedActors;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorDetectedEnhanced, AActor*, DetectedActor, float, Distance);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STILLLOADING_API UCollisionRadarComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCollisionRadarComponent();

    /** 외부에서 호출 가능하도록 BlueprintCallable 추가 */
    UFUNCTION(BlueprintCallable, Category = "Radar")
    void DetectClosestActorInFOV();

    UPROPERTY(BlueprintAssignable, Category = "Radar|Events")
    FOnActorDetectedEnhanced OnActorDetectedEnhanced;

    UPROPERTY(EditAnywhere, Category = "Radar|Debug")
    bool bShowDetectionRadius = false;

    UPROPERTY(EditAnywhere, Category = "Radar|Debug")
    bool bShowFieldOfView = false;

    UPROPERTY(EditAnywhere, Category = "Radar|Settings")
    bool bIsUseRadar = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UMotionWarpingComponent> MotionWarpComponent;

    bool IsInFieldOfView(const AActor* TargetActor) const;
    
protected:
    virtual void BeginPlay() override;

private:
    void DrawDebugVisualization();

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY(VisibleAnywhere, Category = "Radar|Components")
    USphereComponent* DetectionZone = nullptr;

    UPROPERTY(EditAnywhere, Category = "Radar|Settings", meta = (ClampMin = "0.0"))
    float DetectionRadius = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Radar|Settings", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float FieldOfView = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Radar|Settings", meta = (ClampMin = "0.01"))
    float UpdateInterval = 0.2f; // 추가: Timer 간격 (초)

    UPROPERTY()
    TArray<AActor*> NearbyActors;

    FTimerHandle UpdateTimerHandle;
};
