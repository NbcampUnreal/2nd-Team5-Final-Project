#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBoss.generated.h"

class ASLAIBaseCharacter;
class ASLBossCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterDeath, ASLAIBaseCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPatternFinished, ASLAIBaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeveloperBossPatternFinished);

UCLASS()
class STILLLOADING_API ASLDeveloperBoss : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperBoss();
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    ASLAIBaseCharacter* SpawnBossCharacter(TSubclassOf<ASLAIBaseCharacter> BossClass, const FTransform& SpawnTransform);
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void DespawnAllBosses();

    //테스트용 함수
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestSpawnRandomBoss();
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestSpawnAllBosses();
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestKillAllBosses();

    /**
     * @brief 보스 캐릭터가 사망했을 때 호출되는 델리게이트입니다.
     *
     * 이 델리게이트는 보스 캐릭터의 사망 이벤트 발생 시 브로드캐스트됩니다.
     * Blueprint 상호작용을 지원하며, 블루프린트에서 이벤트를 바인딩하여
     * 해당 시점에 추가적인 작업을 수행할 수 있습니다.
     *
     * @category Developer Boss
     */
    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnBossCharacterDeath OnBossCharacterDeath;

    /**
     * @brief 보스 패턴이 완료되었을 때 호출되는 델리게이트입니다.
     *
     * 이 델리게이트는 모든 보스가 소환 해제(Despawn)되거나
     * 보스와 관련된 특정 패턴이 완료되었을 때 브로드캐스트됩니다.
     *
     * Blueprint 상호작용을 지원하며, 블루프린트에서 이벤트를 바인딩하여
     * 해당 시점에 추가적인 작업을 수행할 수 있습니다.
     *
     * @category Developer Boss
     */
    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnBossPatternFinished OnBossPatternFinished;

   
    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnDeveloperBossPatternFinished OnDeveloperBossPatternFinished;

protected:
    UFUNCTION()
    void HandleBossDeath(ASLAIBaseCharacter* DeadBoss);
    
    UFUNCTION()
    void HandlePatternFinished(ASLAIBaseCharacter* Boss);
    
    void RegisterBossEvents(ASLAIBaseCharacter* Boss);
    void UnregisterBossEvents(ASLAIBaseCharacter* Boss);

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Developer Boss")
    TArray<TSubclassOf<ASLAIBaseCharacter>> AvailableBossClasses;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss")
    TArray<ASLAIBaseCharacter*> SpawnedBosses;
};