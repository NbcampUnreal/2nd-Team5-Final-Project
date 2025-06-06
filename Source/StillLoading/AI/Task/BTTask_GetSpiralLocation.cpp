// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetSpiralLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

UBTTask_GetSpiralLocation::UBTTask_GetSpiralLocation()
{
    NodeName = "Get Spiral Location";
    
    // 블랙보드 키 타입 필터 설정
    CurrentIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetSpiralLocation, CurrentIndexKey));
    TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetSpiralLocation, TargetLocationKey));
    IsCompleteKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetSpiralLocation, IsCompleteKey));
    
    // 기본값 설정
    NumberOfTurns = 3.0f;
    TotalPoints = 20;
    MinRadius = 100.0f;
    MaxRadius = 800.0f;
    bUseActorLocation = true;
    FixedCenterLocation = FVector::ZeroVector;
}

EBTNodeResult::Type UBTTask_GetSpiralLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // 현재 인덱스 가져오기
    int32 CurrentIndex = BlackboardComp->GetValueAsInt(CurrentIndexKey.SelectedKeyName);

    // 모든 점을 방문했는지 확인
    if (CurrentIndex >= TotalPoints)
    {
        // 완료 플래그 설정 (키가 설정된 경우)
        if (IsCompleteKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, true);
        }
        return EBTNodeResult::Failed;
    }

    // 중심점 결정
    FVector CenterLocation = FixedCenterLocation;
    if (bUseActorLocation)
    {
        AAIController* AIController = OwnerComp.GetAIOwner();
        APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
        if (!Pawn)
        {
            return EBTNodeResult::Failed;
        }
        CenterLocation = Pawn->GetActorLocation();
    }

    // 현재 인덱스에 해당하는 나선형 위치 계산
    FVector SpiralLocation = CalculateSpiralPoint(CurrentIndex, CenterLocation);

    // 블랙보드에 위치 저장
    BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, SpiralLocation);

    // 인덱스 증가
    BlackboardComp->SetValueAsInt(CurrentIndexKey.SelectedKeyName, CurrentIndex + 1);

    // 완료 플래그 초기화 (키가 설정된 경우)
    if (IsCompleteKey.SelectedKeyName != NAME_None && CurrentIndex == 0)
    {
        BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, false);
    }

    return EBTNodeResult::Succeeded;
}

FVector UBTTask_GetSpiralLocation::CalculateSpiralPoint(int32 Index, const FVector& Center) const
{
    // 0.0 ~ 1.0 사이의 진행도
    float Progress = (float)Index / (float)FMath::Max(1, TotalPoints - 1);
    
    // 각도 계산 (라디안)
    float Angle = Progress * NumberOfTurns * 2.0f * PI;
    
    // 반지름 계산 (선형 보간)
    float Radius = FMath::Lerp(MinRadius, MaxRadius, Progress);
    
    // 2D 좌표 계산
    float X = Center.X + Radius * FMath::Cos(Angle);
    float Y = Center.Y + Radius * FMath::Sin(Angle);
    float Z = Center.Z;
    
    return FVector(X, Y, Z);
}

