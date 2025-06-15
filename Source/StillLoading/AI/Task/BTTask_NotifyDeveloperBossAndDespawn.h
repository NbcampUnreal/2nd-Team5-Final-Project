#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NotifyDeveloperBossAndDespawn.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_NotifyDeveloperBossAndDespawn : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_NotifyDeveloperBossAndDespawn();
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

protected:
    UPROPERTY(EditAnywhere, Category = "Task")
    float DespawnDelay;
    
    UPROPERTY(EditAnywhere, Category = "Task")
    bool bPlayDeathAnimation;

private:
    class ASLDeveloperBoss* FindDeveloperBoss() const;
    void DespawnCharacter(class ASLAIBaseCharacter* Character);

    void DestroyCharacterWeapons(class ASLAIBaseCharacter* Character);
};