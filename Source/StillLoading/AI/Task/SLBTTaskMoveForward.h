// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTaskMoveForward.generated.h"


struct FMoveForwardMemory
{
public:

	void Reset()
	{
		TimeElapsed = 0.0f;
	}
	
	float TimeElapsed;
};

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTTaskMoveForward : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTaskMoveForward();
    
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    
protected:
	virtual uint16 GetInstanceMemorySize() const override;
    
private:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 1.0f;
    
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveDuration = 2.0f;
};
