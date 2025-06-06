// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FlyToHeight.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UBTTask_FlyToHeight : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FlyToHeight();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Flying", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float TargetHeight;
    
	UPROPERTY(EditAnywhere, Category = "Flying", meta = (ClampMin = "50.0", ClampMax = "500.0"))
	float Speed;
    
	UPROPERTY(EditAnywhere, Category = "Flying", meta = (ClampMin = "5.0", ClampMax = "100.0"))
	float AcceptableRadius;
    
	UPROPERTY(EditAnywhere, Category = "Flying")
	bool bStopOnFail;

private:
	bool IsHeightReached(class ASLCompanionCharacter* Companion) const;
};