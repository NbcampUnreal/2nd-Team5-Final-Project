#pragma once

#include "CoreMinimal.h"
#include "Character/Interface/SLBattleInterface.h"
#include "Components/ActorComponent.h"
#include "BattleComponent.generated.h"

class UCapsuleComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBattleComponent : public UActorComponent, public ISLBattleInterface
{
	GENERATED_BODY()

public:
	UBattleComponent();

	UFUNCTION(BlueprintCallable)
	void PerformAttack();

	UPROPERTY(Blueprintable, EditAnywhere, Category="Debug")
	bool bShowDebugLine = true;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	bool bForceMultiplayerMode = false;

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float AttackRange = 100.f;

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float AttackRadius = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	TEnumAsByte<ECollisionChannel> EnemyChannel = ECC_GameTraceChannel2;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPerformAttack();

	UFUNCTION()
	void DoAttack();
};
