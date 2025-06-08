#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_2DRotateToTarget.generated.h"

struct F2DRotateTaskMemory
{
public:
    bool IsValid() const
    {
        return OwningPawn.IsValid() && TargetActor.IsValid();
    }

    void Reset()
    {
        OwningPawn.Reset();
        TargetActor.Reset();
        ElapsedTime = 0.0f;
        StartRotation = FRotator::ZeroRotator;
        TargetRotation = FRotator::ZeroRotator;
    }
    
    TWeakObjectPtr<APawn> OwningPawn;
    TWeakObjectPtr<AActor> TargetActor;
    float ElapsedTime;
    FRotator StartRotation;
    FRotator TargetRotation;
};

UCLASS()
class STILLLOADING_API USLBTTask_2DRotateToTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    USLBTTask_2DRotateToTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
    virtual uint16 GetInstanceMemorySize() const override;
    virtual FString GetStaticDescription() const override;

private:
    float CalculateWorldBasedTargetRotation(const FVector& FromLocation, const FVector& ToLocation) const;
    bool IsAtTargetRotation(float CurrentYaw, float TargetYaw, float Tolerance) const;

    UPROPERTY(EditAnywhere, Category = "2D Rotation")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "2D Rotation")
    float RotationSpeed;

    UPROPERTY(EditAnywhere, Category = "2D Rotation")
    float AngleTolerance;

    UPROPERTY(EditAnywhere, Category = "2D Rotation")
    float MaxRotationTime;

    UPROPERTY(EditAnywhere, Category = "2D Rotation")
    bool bInstantRotation;
};