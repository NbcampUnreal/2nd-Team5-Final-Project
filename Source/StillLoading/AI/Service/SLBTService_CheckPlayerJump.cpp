// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBTService_CheckPlayerJump.h"

#include "AIController.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


USLBTService_CheckPlayerJump::USLBTService_CheckPlayerJump()
{
    NodeName = TEXT("Check Player Jump");
    Interval = 0.1f; // 0.1초마다 체크
    RandomDeviation = 0.02f; // 약간의 랜덤 편차
    bPreviousPlayerJumpState = false;
    
    // 기본 블랙보드 키 설정
    PlayerIsJumpingKey.SelectedKeyName = FName("PlayerIsJumping");
    PlayerStartedJumpingKey.SelectedKeyName = FName("PlayerStartedJumping");
}

void USLBTService_CheckPlayerJump::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return;
    }

    ASLDoppelgangerCharacter* DoppelgangerCharacter = Cast<ASLDoppelgangerCharacter>(AIController->GetPawn());
    if (!DoppelgangerCharacter)
    {
        return;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // 현재 플레이어 점프 상태 확인
    bool bCurrentPlayerJumpState = DoppelgangerCharacter->IsPlayerJumping();

    // 블랙보드에 현재 점프 상태 설정
    BlackboardComp->SetValueAsBool(PlayerIsJumpingKey.SelectedKeyName, bCurrentPlayerJumpState);

    // 플레이어가 새로 점프를 시작했는지 확인
    bool bPlayerStartedJumping = bCurrentPlayerJumpState && !bPreviousPlayerJumpState;
    BlackboardComp->SetValueAsBool(PlayerStartedJumpingKey.SelectedKeyName, bPlayerStartedJumping);

    // 이전 상태 업데이트
    bPreviousPlayerJumpState = bCurrentPlayerJumpState;

    // 디버그 로그
    if (bPlayerStartedJumping)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTService: Player started jumping!"));
    }
}
