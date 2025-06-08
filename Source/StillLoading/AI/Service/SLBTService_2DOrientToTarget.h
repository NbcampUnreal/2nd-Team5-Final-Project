#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SLBTService_2DOrientToTarget.generated.h"

UCLASS()
class STILLLOADING_API USLBTService_2DOrientToTarget : public UBTService
{
	GENERATED_BODY()

public:
	USLBTService_2DOrientToTarget();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float CalculateWorldBasedTargetRotation(const FVector& FromLocation, const FVector& ToLocation) const;
	bool ShouldUpdateRotation(float CurrentYaw, float TargetYaw, float Threshold) const;

	UPROPERTY(EditAnywhere, Category = "2D Rotation")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "2D Rotation")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, Category = "2D Rotation")
	bool bInstantRotation;

	UPROPERTY(EditAnywhere, Category = "2D Rotation")
	float AngleChangeThreshold;

	UPROPERTY(EditAnywhere, Category = "2D Rotation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RotationSmoothness;
};