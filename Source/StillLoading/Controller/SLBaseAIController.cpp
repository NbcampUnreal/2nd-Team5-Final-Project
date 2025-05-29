// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SLBaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"

ASLBaseAIController::ASLBaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfig_Sight");
	AISenseConfig_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>("AISenseConfig_Damage");
	
	// 감지 설정 구성
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true; // 적 감지 활성화
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false; // 아군 감지 비활성화
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false; // 중립 대상 감지 비활성화
	
	// 시야 파라미터 설정
	AISenseConfig_Sight->SightRadius = 1000.f; // 최대 감지 거리
	AISenseConfig_Sight->LoseSightRadius = 1100.f; // 감지 상실 거리
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f; // 완전한 360도 시야각

	// AI 감지 컴포넌트 생성 및 설정
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Sight); 
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Damage);
	AIPerceptionComponent->SetDominantSense(AISenseConfig_Sight->GetSenseImplementation()); // 시야 감각을 주요 감각으로 설정
	
	// 감지 업데이트 이벤트 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnAIPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this,&ThisClass::OnTargetPerceptionForgotten);

	bIsHostileToOtherAI = true;
}

void ASLBaseAIController::UpdateSightRadius(float SightRadius, float LoseSightRadius)
{
	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();
	if (!PerceptionComp)
	{
		return;
	}
    
	// 시야 설정 가져오기
	UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(PerceptionComp->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));

	if (SightConfig)
	{
		// 전투 모드일 때 시야 확장
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
        
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	}
}

void ASLBaseAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (Stimulus.WasSuccessfullySensed() && Actor)
		{
			if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
			{
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
		else
		{
			GetBlackboardComponent()->ClearValue(FName("TargetActor"));
		}
	}
}

// 이전에 감지한 액터를 잊었을때 호출
void ASLBaseAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
}

void ASLBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	checkf(BehaviorTreeToRun, TEXT("BehaviorTreeToRun is nullptr. Current object: %s"), *GetName());
	
	RunBehaviorTree(BehaviorTreeToRun);
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsVector(FName("StartLocation"), InPawn->GetActorLocation());

		ASLAIBaseCharacter* AiCharacter = Cast<ASLAIBaseCharacter>(InPawn);
		BlackboardComponent->SetValueAsFloat(FName("DefaultMaxWalkSpeed"), AiCharacter->GetCharacterMovement()->MaxWalkSpeed);
		BlackboardComponent->SetValueAsFloat(FName("MaxHealth"), AiCharacter->GetMaxHealth());
		BlackboardComponent->SetValueAsFloat(FName("CurrentHealth"), AiCharacter->GetCurrentHealth());
	}
}

void ASLBaseAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

		switch (DetourCrowdAvoidanceQuality)
		{
		case 1: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
		case 2: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
		case 3: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
		case 4: CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);   break;
		default: break;
		}

		CrowdComp->SetAvoidanceGroup(1);
		CrowdComp->SetGroupsToAvoid(1);
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

void ASLBaseAIController::SetAITeamId(const FGenericTeamId& NewTeamID)
{
	AAIController::SetGenericTeamId(NewTeamID);
}
