// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_PlayRandomMontage.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

UBTTask_PlayRandomMontage::UBTTask_PlayRandomMontage()
{
    NodeName = TEXT("Play Random Montage");
    // 태스크 인스턴스 생성 설정
    bCreateNodeInstance = true;
    
    // 몽타주 종료 델리게이트 설정
    MontageEndedDelegate.BindUObject(this, &UBTTask_PlayRandomMontage::OnMontageEnded);
}

EBTNodeResult::Type UBTTask_PlayRandomMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러와 캐릭터 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AIController->GetPawn();
    ACharacter* Character = Cast<ACharacter>(Pawn);
    if (!Character)
    {
        return EBTNodeResult::Failed;
    }

    // 애니메이션 인스턴스 가져오기
    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        return EBTNodeResult::Failed;
    }

    // 재생할 몽타주가 있는지 확인
    if (Montages.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    // 랜덤 몽타주 선택
    int32 RandomIndex = FMath::RandRange(0, Montages.Num() - 1);
    UAnimMontage* MontageToPlay = Montages[RandomIndex];
    
    if (!MontageToPlay)
    {
        return EBTNodeResult::Failed;
    }

    // 현재 재생 중인 몽타주 저장
    CurrentPlayingMontage = MontageToPlay;
    
    // 현재 동작 중인 BehaviorTreeComponent 저장
    CurrentOwnerComp = &OwnerComp;
    
    // 몽타주 재생
    float Duration = AnimInstance->Montage_Play(MontageToPlay, PlayRate);
    
    // 몽타주 종료 델리게이트 연결
    if (Duration > 0.f)
    {
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
        return EBTNodeResult::InProgress;  
    }
    
    return EBTNodeResult::Failed;
}

void UBTTask_PlayRandomMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    // 태스크 종료 시 정리
    CurrentOwnerComp = nullptr;
    CurrentPlayingMontage = nullptr;
    
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_PlayRandomMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 몽타주 종료 시 태스크 완료
    if (CurrentOwnerComp && Montage == CurrentPlayingMontage)
    {
        UE_LOG(LogTemp, Display, TEXT("Montage '%s' ended (Interrupted: %s)"), 
            *Montage->GetName(), bInterrupted ? TEXT("true") : TEXT("false"));
        
        // 태스크 완료 처리
        FinishLatentTask(*CurrentOwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
    }
}