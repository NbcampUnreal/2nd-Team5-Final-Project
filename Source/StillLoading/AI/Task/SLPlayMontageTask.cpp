// Fill out your copyright notice in the Description page of Project Settings.

#include "SLPlayMontageTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

USLPlayMontageTask::USLPlayMontageTask()
{
    // 노드 이름 설정
    NodeName = "Play Montage with Variable Speed";
    
    // 기본값 설정
    bWaitForMontageToFinish = true;
    MontageToPlay = nullptr;
    PlayRate = 1.0f;
    BlendInTime = 0.25f;
    SectionName = NAME_None;
    
    // 내부 변수 초기화
    CurrentMontage = nullptr;
    BTComp = nullptr;
    AnimInstance = nullptr;
}

EBTNodeResult::Type USLPlayMontageTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Play Montage Task: AI Controller가 없습니다."));
        return EBTNodeResult::Failed;
    }

    // 캐릭터 가져오기
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Play Montage Task: Character가 없습니다."));
        return EBTNodeResult::Failed;
    }

    // 애님 인스턴스 가져오기
    UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance();
    if (!AnimInst)
    {
        UE_LOG(LogTemp, Warning, TEXT("Play Montage Task: AnimInstance가 없습니다."));
        return EBTNodeResult::Failed;
    }

    // 이전 델리게이트가 있으면 제거 (안전성을 위해)
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.RemoveDynamic(this, &USLPlayMontageTask::OnMontageEnded);
    }

    // 몽타주가 유효한지 확인
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("Play Montage Task: 유효한 몽타주가 없습니다."));
        return EBTNodeResult::Failed;
    }

    // 재생 속도 제한 (합리적인 범위로)
    float FinalPlayRate = FMath::Clamp(PlayRate, 0.1f, 5.0f);

    // 레퍼런스 저장
    CurrentMontage = MontageToPlay;
    BTComp = &OwnerComp;
    AnimInstance = AnimInst;

    // 몽타주 재생
    float Duration = AnimInst->Montage_Play(MontageToPlay, FinalPlayRate, EMontagePlayReturnType::MontageLength, BlendInTime);
    
    // 특정 섹션으로 이동 (설정된 경우)
    if (SectionName != NAME_None)
    {
        AnimInst->Montage_JumpToSection(SectionName, MontageToPlay);
    }

    // 재생 실패 시 처리
    if (Duration <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Play Montage Task: 몽타주 재생 실패 - %s"), *MontageToPlay->GetName());
        // 레퍼런스 정리
        CurrentMontage = nullptr;
        BTComp = nullptr;
        AnimInstance = nullptr;
        return EBTNodeResult::Failed;
    }

    // 몽타주 완료를 기다리지 않는 경우
    if (!bWaitForMontageToFinish)
    {
        // 레퍼런스 정리 (기다리지 않을 경우 필요없음)
        CurrentMontage = nullptr;
        BTComp = nullptr;
        AnimInstance = nullptr;
        return EBTNodeResult::Succeeded;
    }

    // 몽타주 종료 이벤트 등록
    AnimInst->OnMontageEnded.AddDynamic(this, &USLPlayMontageTask::OnMontageEnded);

    // 진행 중으로 반환 (몽타주 종료 시 완료됨)
    return EBTNodeResult::InProgress;
}

void USLPlayMontageTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    // 몽타주 정리 및 델리게이트 제거
    if (AnimInstance)
    {
        // 몽타주가 재생 중이면 중지
        if (CurrentMontage && AnimInstance->Montage_IsPlaying(CurrentMontage))
        {
            AnimInstance->Montage_Stop(BlendInTime, CurrentMontage);
        }
        
        // 델리게이트 제거
        AnimInstance->OnMontageEnded.RemoveDynamic(this, &USLPlayMontageTask::OnMontageEnded);
    }

    // 레퍼런스 정리
    CurrentMontage = nullptr;
    BTComp = nullptr;
    AnimInstance = nullptr;

    // 부모 클래스 처리
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void USLPlayMontageTask::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 현재 재생 중인 몽타주와 일치하는지 확인
    if (Montage != CurrentMontage || !BTComp)
    {
        return;
    }

    // 델리게이트 제거
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.RemoveDynamic(this, &USLPlayMontageTask::OnMontageEnded);
    }

    // 레퍼런스 백업 (정리 전에)
    UBehaviorTreeComponent* OwnerComp = BTComp;
    
    // 레퍼런스 정리
    CurrentMontage = nullptr;
    BTComp = nullptr;
    AnimInstance = nullptr;

    // 태스크 완료
    FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

EBTNodeResult::Type USLPlayMontageTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 몽타주 중지 및 정리
    if (AnimInstance)
    {
        // 현재 몽타주가 재생 중이면 중지
        if (CurrentMontage && AnimInstance->Montage_IsPlaying(CurrentMontage))
        {
            AnimInstance->Montage_Stop(BlendInTime, CurrentMontage);
        }
        
        // 델리게이트 제거
        AnimInstance->OnMontageEnded.RemoveDynamic(this, &USLPlayMontageTask::OnMontageEnded);
    }

    // 레퍼런스 정리
    CurrentMontage = nullptr;
    BTComp = nullptr;
    AnimInstance = nullptr;

    // 중단됨으로 반환
    return EBTNodeResult::Aborted;
}

FString USLPlayMontageTask::GetStaticDescription() const
{
    // 설명 생성 (BP 에디터에서 표시됨)
    FString MontageDesc = FString::Printf(TEXT("몽타주: %s"), 
        MontageToPlay ? *MontageToPlay->GetName() : TEXT("없음"));
    
    FString PlayRateDesc = FString::Printf(TEXT("\n재생 속도: %.2f"), PlayRate);

    FString SectionDesc = SectionName != NAME_None ?
        FString::Printf(TEXT("\n섹션: %s"), *SectionName.ToString()) :
        TEXT("");

    FString WaitDesc = bWaitForMontageToFinish ?
        TEXT("\n완료 대기") :
        TEXT("\n완료 대기 안함");

    return FString::Printf(TEXT("%s%s%s%s"), *MontageDesc, *PlayRateDesc, *SectionDesc, *WaitDesc);
}