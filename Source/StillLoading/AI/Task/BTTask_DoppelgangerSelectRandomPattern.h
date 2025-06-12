#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"
#include "BTTask_DoppelgangerSelectRandomPattern.generated.h"

UENUM(BlueprintType)
enum class EDoppelgangerPatternType : uint8
{
    EDPT_All                    UMETA(DisplayName = "All Patterns"),
    EDPT_ComboAttack           UMETA(DisplayName = "Combo Attack"),
    EDPT_SpecialAttack         UMETA(DisplayName = "Special Attack"),
    EDPT_Defense               UMETA(DisplayName = "Defense"),
    EDPT_CustomTags            UMETA(DisplayName = "Custom Tags")
};

UCLASS()
class STILLLOADING_API UBTTask_DoppelgangerSelectRandomPattern : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_DoppelgangerSelectRandomPattern();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

protected:
    bool SelectAndExecutePattern(ASLDoppelgangerCharacter* DoppelgangerCharacter);
    FGameplayTagContainer GetPatternsByType(ASLDoppelgangerCharacter* DoppelgangerCharacter) const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern Selection")
    EDoppelgangerPatternType PatternType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern Selection", meta = (EditCondition = "PatternType == EDoppelgangerPatternType::EDPT_CustomTags"))
    FGameplayTagContainer CustomPatternTags;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
    FBlackboardKeySelector SelectedPatternKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    bool bSetAttackingState;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    bool bLogSelectedPattern;
};