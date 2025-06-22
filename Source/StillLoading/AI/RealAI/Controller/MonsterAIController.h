#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonsterAIController.generated.h"

class AAISquadManager;
class UWidgetComponent;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;
class UBehaviorTreeComponent;

UCLASS()
class STILLLOADING_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

	UFUNCTION()
	void ToggleLockOnWidget(bool bIsLockOnWidget);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LockOnWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> LockOnWidgetFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> LockOnWidgetBack;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
