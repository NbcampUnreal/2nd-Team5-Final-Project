#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLBattleManagerInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USLBattleManagerInterface : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API ISLBattleManagerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void StartBattle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void EndBattle();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void PauseBattle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void ResumeBattle();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	bool IsBattleInProgress() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	int32 GetCurrentWaveNumber() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	int32 GetTotalWaveCount() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	int32 GetAliveEnemyCount() const;
};