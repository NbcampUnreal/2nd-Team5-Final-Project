#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SLBattlePlayerState.generated.h"

UCLASS()
class STILLLOADING_API ASLBattlePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASLBattlePlayerState();
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Stat")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_IsWalking, BlueprintReadOnly, Category = "Stat")
	bool bIsWalking;

	void SetHealth(float NewHealth);
	void SetWalking(bool bNewWalking);
	
	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_IsWalking();
};
