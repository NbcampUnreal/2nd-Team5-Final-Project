#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UI/Struct/SLInGameDelegateBuffers.h"
#include "SLBattlePlayerState.generated.h"

class UCharacterStatData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHpChangedLocal, float, MaxHp, float, CurrentHp);

UCLASS()
class STILLLOADING_API ASLBattlePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASLBattlePlayerState();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable)
	void DecreaseHealth(float Amount);
	UFUNCTION()
	void SetWalking(bool bNewWalking);
	UFUNCTION()
	void SetMaxSpeed(float NewMaxSpeed);
	UFUNCTION()
	void IncreaseBurningGage(float Amount);
	UFUNCTION()
	void ResetState();

	UFUNCTION()
	FSLSpecialValueDelegateBuffer& GetGageDelegate() { return GageDelegate; }
	UFUNCTION()
	FSLPlayerHpDelegateBuffer& GetHPDelegate() { return HPDelegate; }

	UFUNCTION()
	FORCEINLINE float GetBurningGage() const { return BurningGage; }

	UPROPERTY(BlueprintAssignable)
	FOnPlayerHpChangedLocal OnPlayerHpChanged;

protected:
	UFUNCTION()
	void OnRep_Health();
	UFUNCTION()
	void OnRep_IsWalking();
	UFUNCTION()
	void OnRep_BurningGage();

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
	
	UPROPERTY(ReplicatedUsing = OnRep_BurningGage, BlueprintReadOnly, Category = "Stat")
	float BurningGage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float MaxBurningGage = 100.f;

private:
	UFUNCTION()
	void UpdateGauge();
	UFUNCTION()
	void OnPlayerEmpoweredStateChanged(bool bIsEmpowered);

	FTimerHandle GaugeUpdateTimerHandle;

	UPROPERTY()
	FSLSpecialValueDelegateBuffer GageDelegate;
	UPROPERTY()
	FSLPlayerHpDelegateBuffer HPDelegate;
};
