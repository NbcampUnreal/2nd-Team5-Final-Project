// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTaskRotateToFaceTarget.generated.h"


struct FRotateToFaceTargetTaskMemory
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
	}
	
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;
	float ElapsedTime;
};

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTTaskRotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTaskRotateToFaceTarget();

protected:
	//~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const;
	
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Task")
	float MaxRotationTime;
	
	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetToFaceKey;
};
