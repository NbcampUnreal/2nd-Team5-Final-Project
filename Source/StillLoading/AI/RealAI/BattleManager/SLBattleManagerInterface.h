#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLBattleManagerInterface.generated.h"

class ASLSwarmSpawner;

UINTERFACE(MinimalAPI, Blueprintable)
class USLBattleManagerInterface : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API ISLBattleManagerInterface
{
	GENERATED_BODY()

public:
	/**
	 * 지정된 스포너들을 활성화하여 특정 웨이브를 시작시킵니다.
	 * @param WaveIndex         스포너 내부의 웨이브 데이터 인덱스
	 * @param SpawnersToActivate 이 웨이브에서 활성화할 스포너 객체 목록
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void StartWave(const TArray<ASLSwarmSpawner*>& SpawnersToActivate, int32 WaveIndex = 0);

	/**
    * 현재 월드에 배치된 모든 유닛을 스포너의 풀로 되돌리고, 진행 중인 웨이브 스폰을 중지합니다.
    * EndBattle과 달리 전투 상태 자체는 유지합니다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void StopBattle();

	/**
	 * 지정된 스포너들을 무한 리스폰 모드로 전환시킵니다.
	 * @param SpawnersToActivate 무한 리스폰 모드를 적용할 스포너 목록
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void StartInfiniteSpawnMode(const TArray<ASLSwarmSpawner*>& SpawnersToActivate);

	/**
	 * 진행 중인 모든 스폰 활동을 중단하고 전투를 종료합니다.
	 * @param bPlayerWon 전투의 승패 결과
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Control")
	void EndBattle(bool bPlayerWon);

	/**
	 * 전투 시작 이후 스폰된 유닛의 총 누적 수를 반환합니다.
	 * @return 누적 스폰 유닛 수
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	int32 GetTotalSpawnedUnitCount() const;

	/**
	* 스폰된 유닛 카운트를 초기화 합니다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	void ResetSpawnedUnitCount();

	/**
	 * 현재 전투가 진행 중인지 여부를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Battle Management|Queries")
	bool IsBattleInProgress() const;
};
