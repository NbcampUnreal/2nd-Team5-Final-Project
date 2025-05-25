// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossCharacter.h"

#include "AIController.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "AnimInstances/SLBossAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"


ASLBossCharacter::ASLBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	BossAttackPattern = EBossAttackPattern::EBAP_None;

	//처형 안되게 수정해야함
	bCanBeExecuted = true;
}

void ASLBossCharacter::SetBossAttackPattern(EBossAttackPattern NewPattern)
{
	BossAttackPattern = NewPattern;

	if (USLBossAnimInstance* SLAIAnimInstance = Cast<USLBossAnimInstance>(AnimInstancePtr.Get()))
	{
		SLAIAnimInstance->SetBossAttackPattern(NewPattern);
	}
}

void ASLBossCharacter::FindTargetPoint()
{
	// 레벨에서 TargetPoint 액터 찾기
	TArray<AActor*> FoundTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundTargetPoints);
    
	// 찾은 TargetPoint가 있으면 설정
	if (FoundTargetPoints.Num() > 0)
	{
		TargetPoint = FoundTargetPoints[0];
		UE_LOG(LogTemp, Display, TEXT("Found TargetPoint: %s"), *TargetPoint->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No TargetPoint found in level!"));
	}
}



void ASLBossCharacter::SetTargetPointToBlackboard()
{
	if (!TargetPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No TargetPoint set. Call FindTargetPoint first."));
		return;
	}
    
	
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (Blackboard)
		{
			Blackboard->SetValueAsObject("TargetPoint", TargetPoint);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController Is null"));
	}
}

void ASLBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	FindTargetPoint();
	SetTargetPointToBlackboard();

	MaxHealth = 500.0f;
	CurrentHealth = MaxHealth;
}

EBossAttackPattern ASLBossCharacter::SelectRandomPattern(float DistanceToTarget, const TArray<EBossAttackPattern>& CloseRangePatterns, const TArray<EBossAttackPattern>& LongRangePatterns, float DistanceThreshold)
{
    if (DistanceToTarget >= DistanceThreshold)
    {
        // 원거리 패턴
        TArray<EBossAttackPattern> AvailableLongRangePatterns = LongRangePatterns;
        
        // 이전에 사용한 패턴이 있고, 목록에 포함되어 있다면 제거
        if (LastLongRangePattern != EBossAttackPattern::EBAP_None && 
            AvailableLongRangePatterns.Contains(LastLongRangePattern))
        {
            AvailableLongRangePatterns.Remove(LastLongRangePattern);
        }
        
        // 사용 가능한 패턴이 없다면 전체 목록에서 선택
        if (AvailableLongRangePatterns.Num() == 0)
        {
            AvailableLongRangePatterns = LongRangePatterns;
        }
        
        // 랜덤 선택
        int32 RandomIndex = FMath::RandRange(0, AvailableLongRangePatterns.Num() - 1);
        EBossAttackPattern SelectedPattern = AvailableLongRangePatterns[RandomIndex];
        
        LastLongRangePattern = SelectedPattern;
        
        UE_LOG(LogTemp, Display, TEXT("Selected long range pattern: %s"), *UEnum::GetValueAsString(SelectedPattern));
        return SelectedPattern;
    }
    else
    {
    	if (USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
    	{
    		if (AnimInstance->IsTargetBehindCharacter(120.0f)) // 90도 각도로 뒤에 있는지 확인
    		{
    			return EBossAttackPattern::EBAP_Attack_04;
    		}
    	}
    
        // 근거리 패턴
        if (CloseRangePatterns.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No close range patterns available!"));
            return EBossAttackPattern::EBAP_Attack_01; // 기본값
        }
        
        int32 RandomIndex = FMath::RandRange(0, CloseRangePatterns.Num() - 1);
        EBossAttackPattern SelectedPattern = CloseRangePatterns[RandomIndex];
        
        return SelectedPattern;
    }
}