#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SLBattlePlayerState.generated.h"

class UCharacterStatData;

UCLASS()
class STILLLOADING_API ASLBattlePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASLBattlePlayerState();

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void SetHealth(float NewHealth);
	UFUNCTION()
	void DecreaseHealth(float Amount);
	UFUNCTION()
	void SetWalking(bool bNewWalking);
	UFUNCTION()
	void SetMaxSpeed(float NewMaxSpeed);
	UFUNCTION()
	void IncreaseBurningGage(float Amount);

	UFUNCTION()
	FORCEINLINE float GetBurningGage() const { return BurningGage; }

protected:
	UFUNCTION()
	void OnRep_Health();
	UFUNCTION()
	void OnRep_IsWalking();

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Stat")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_IsWalking, BlueprintReadOnly, Category = "Stat")
	bool bIsWalking;

	UPROPERTY()
	float WalkingSpeed = 250.f;
	UPROPERTY()
	float MaxSpeed = 600.f;
	UPROPERTY()
	float JumpZVelocity = 1000.f;
	UPROPERTY()
	float BurningGage = 0.f;
};
