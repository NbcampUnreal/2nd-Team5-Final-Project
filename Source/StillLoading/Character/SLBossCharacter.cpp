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
			UE_LOG(LogTemp, Display, TEXT("TargetPoint set to blackboard: %s"), *TargetPoint->GetName());
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
}
