#include "BTTask_DoppelgangerSelectRandomPattern.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"

UBTTask_DoppelgangerSelectRandomPattern::UBTTask_DoppelgangerSelectRandomPattern()
{
    NodeName = "Doppelganger Select Random Pattern";
    
    PatternType = EDoppelgangerPatternType::EDPT_All;
    bSetAttackingState = true;
    bLogSelectedPattern = true;
    
    // 블랙보드 키 설정
    SelectedPatternKey.AllowNoneAsValue(true);
}

EBTNodeResult::Type UBTTask_DoppelgangerSelectRandomPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_DoppelgangerSelectRandomPattern: No AI Controller"));
        return EBTNodeResult::Failed;
    }

    ASLDoppelgangerCharacter* DoppelgangerCharacter = Cast<ASLDoppelgangerCharacter>(AIController->GetPawn());
    if (!DoppelgangerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_DoppelgangerSelectRandomPattern: Pawn is not a Doppelganger Character"));
        return EBTNodeResult::Failed;
    }

    // 패턴 선택 및 실행
    if (SelectAndExecutePattern(DoppelgangerCharacter))
    {
        // 공격 상태로 설정
        if (bSetAttackingState)
        {
            DoppelgangerCharacter->SetIsAttacking(true);
        }

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

bool UBTTask_DoppelgangerSelectRandomPattern::SelectAndExecutePattern(ASLDoppelgangerCharacter* DoppelgangerCharacter)
{
    if (!DoppelgangerCharacter)
    {
        return false;
    }

    EDoppelgangerActionPattern SelectedPattern = EDoppelgangerActionPattern::EDAP_None;

    // 패턴 타입에 따라 선택
    switch (PatternType)
    {
    case EDoppelgangerPatternType::EDPT_All:
        SelectedPattern = DoppelgangerCharacter->SelectRandomPattern();
        break;

    case EDoppelgangerPatternType::EDPT_ComboAttack:
        {
            FGameplayTagContainer ComboPatterns = DoppelgangerCharacter->GetComboAttackPatterns();
            SelectedPattern = DoppelgangerCharacter->SelectRandomPatternFromTags(ComboPatterns);
        }
        break;

    case EDoppelgangerPatternType::EDPT_SpecialAttack:
        {
            FGameplayTagContainer SpecialPatterns = DoppelgangerCharacter->GetSpecialAttackPatterns();
            SelectedPattern = DoppelgangerCharacter->SelectRandomPatternFromTags(SpecialPatterns);
        }
        break;

    case EDoppelgangerPatternType::EDPT_Defense:
        {
            FGameplayTagContainer DefensePatterns = DoppelgangerCharacter->GetDefensePatterns();
            SelectedPattern = DoppelgangerCharacter->SelectRandomPatternFromTags(DefensePatterns);
        }
        break;

    case EDoppelgangerPatternType::EDPT_CustomTags:
        {
            if (!CustomPatternTags.IsEmpty())
            {
                SelectedPattern = DoppelgangerCharacter->SelectRandomPatternFromTags(CustomPatternTags);
            }
        }
        break;
    }

    // 패턴이 선택되었는지 확인
    if (SelectedPattern == EDoppelgangerActionPattern::EDAP_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_DoppelgangerSelectRandomPattern: No pattern selected"));
        return false;
    }

    // 선택된 패턴을 캐릭터에 설정
    DoppelgangerCharacter->SetCurrentActionPattern(SelectedPattern);

    // 블랙보드에 선택된 패턴 저장 (옵션)
    if (SelectedPatternKey.SelectedKeyName != NAME_None)
    {
        if (AAIController* AIController = Cast<AAIController>(DoppelgangerCharacter->GetController()))
        {
            if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
            {
                BlackboardComp->SetValueAsEnum(SelectedPatternKey.SelectedKeyName, static_cast<uint8>(SelectedPattern));
            }
        }
    }

    // 로깅
    if (bLogSelectedPattern)
    {
        UE_LOG(LogTemp, Display, TEXT("Doppelganger Selected Pattern: %s"), *UEnum::GetValueAsString(SelectedPattern));
    }

    return true;
}

FGameplayTagContainer UBTTask_DoppelgangerSelectRandomPattern::GetPatternsByType(ASLDoppelgangerCharacter* DoppelgangerCharacter) const
{
    if (!DoppelgangerCharacter)
    {
        return FGameplayTagContainer();
    }

    switch (PatternType)
    {
    case EDoppelgangerPatternType::EDPT_ComboAttack:
        return DoppelgangerCharacter->GetComboAttackPatterns();

    case EDoppelgangerPatternType::EDPT_SpecialAttack:
        return DoppelgangerCharacter->GetSpecialAttackPatterns();

    case EDoppelgangerPatternType::EDPT_Defense:
        return DoppelgangerCharacter->GetDefensePatterns();

    case EDoppelgangerPatternType::EDPT_CustomTags:
        return CustomPatternTags;

    case EDoppelgangerPatternType::EDPT_All:
    default:
        return DoppelgangerCharacter->GetAllAttackPatterns();
    }
}

FString UBTTask_DoppelgangerSelectRandomPattern::GetStaticDescription() const
{
    FString PatternTypeStr;
    
    switch (PatternType)
    {
    case EDoppelgangerPatternType::EDPT_All:
        PatternTypeStr = "All Patterns";
        break;
    case EDoppelgangerPatternType::EDPT_ComboAttack:
        PatternTypeStr = "Combo Attack Patterns";
        break;
    case EDoppelgangerPatternType::EDPT_SpecialAttack:
        PatternTypeStr = "Special Attack Patterns";
        break;
    case EDoppelgangerPatternType::EDPT_Defense:
        PatternTypeStr = "Defense Patterns";
        break;
    case EDoppelgangerPatternType::EDPT_CustomTags:
        PatternTypeStr = FString::Printf(TEXT("Custom Tags (%d)"), CustomPatternTags.Num());
        break;
    }

    return FString::Printf(TEXT("Select Random Pattern from: %s"), *PatternTypeStr);
}