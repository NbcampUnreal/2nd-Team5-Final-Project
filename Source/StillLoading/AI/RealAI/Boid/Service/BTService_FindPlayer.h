#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindPlayer.generated.h"

class ASwarmManager;

UCLASS()
class STILLLOADING_API UBTService_FindPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
	bool IsInFieldOfView(const AActor* TargetActor, const APawn* ControlledPawn, const ASwarmManager* SwarmManager);

	UPROPERTY(EditAnywhere, Category="AI")
	float ForgettingTime = 2.0f;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

private:
	bool CheckTag(const UBehaviorTreeComponent& OwnerComp);
};
