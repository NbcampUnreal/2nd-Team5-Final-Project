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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(AActor* Target);

protected:
	virtual void BeginPlay() override;
	virtual void ReceiveBattleDamage_Implementation(float DamageAmount) override;

private:
	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						 bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Attack(AActor* Target);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UCapsuleComponent> AttackCollision;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> EnemyChannel = ECC_GameTraceChannel2;
};
