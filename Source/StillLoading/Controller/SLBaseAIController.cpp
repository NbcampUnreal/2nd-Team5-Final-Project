// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/SLBaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogAITargeting, Log, All);

FTargetInfo::FTargetInfo()
{
	Distance = 0.0f;
	ThreatLevel = 0.0f;
	LastSeenTime = 0.0f;
}

ASLBaseAIController::ASLBaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfig_Sight");
	AISenseConfig_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>("AISenseConfig_Damage");
	
	// 감지 설정 구성
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	
	// 시야 파라미터 설정
	AISenseConfig_Sight->SightRadius = 1000.f;
	AISenseConfig_Sight->LoseSightRadius = 1100.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f;

	// AI 감지 컴포넌트 생성 및 설정
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Sight); 
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Damage);
	AIPerceptionComponent->SetDominantSense(AISenseConfig_Sight->GetSenseImplementation());
	
	// 감지 업데이트 이벤트 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnAIPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetPerceptionForgotten);

	bIsHostileToOtherAI = true;
	bEnableDetourCrowdAvoidance = true;
	DetourCrowdAvoidanceQuality = 4;
	CollisionQueryRange = 600.f;
	
	// 타겟 평가 관련 초기화
	TargetEvaluationInterval = 0.5f;
	MaxTargetSwitchDistance = 500.0f;
	DamageThreatMultiplier = 2.0f;
	LastTargetEvaluationTime = 0.0f;
}

void ASLBaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 죽은 타겟 정리
	CleanupDeadTargets();
	
	// 주기적으로 타겟 재평가
	if (GetWorld()->GetTimeSeconds() - LastTargetEvaluationTime > TargetEvaluationInterval)
	{
		UpdateTargetEvaluation();
		LastTargetEvaluationTime = GetWorld()->GetTimeSeconds();
	}
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
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
        
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	}
}

void ASLBaseAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}
	
	// 죽은 액터는 무시
	if (!IsActorAlive(Actor))
	{
		return;
	}
	
	// 팀 태도 확인
	ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*Actor);
	
	if (Stimulus.WasSuccessfullySensed() && Attitude == ETeamAttitude::Hostile)
	{
		UE_LOG(LogAITargeting, Log, TEXT("[%s] Target Detected: %s (Distance: %.1f, Attitude: Hostile)"), 
			*GetName(), *Actor->GetName(), FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation()));
		
		// 타겟 정보 추가 또는 업데이트
		AddOrUpdateTarget(Actor);
		
		// 데미지 자극인 경우 위협도 증가
		if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
		{
			if (FTargetInfo* TargetInfo = PotentialTargets.Find(Actor))
			{
				float OldThreatLevel = TargetInfo->ThreatLevel;
				TargetInfo->ThreatLevel *= DamageThreatMultiplier;
				UE_LOG(LogAITargeting, Log, TEXT("[%s] Damage from %s - Threat Level: %.2f -> %.2f"), 
					*GetName(), *Actor->GetName(), OldThreatLevel, TargetInfo->ThreatLevel);
			}
		}
		
		// 즉시 타겟 재평가
		UpdateTargetEvaluation();
	}
	else if (!Stimulus.WasSuccessfullySensed() && Attitude == ETeamAttitude::Hostile)
	{
		UE_LOG(LogAITargeting, Log, TEXT("[%s] Target Lost: %s"), *GetName(), *Actor->GetName());
		
		// 시야에서 사라진 적 처리
		if (FTargetInfo* TargetInfo = PotentialTargets.Find(Actor))
		{
			TargetInfo->LastSeenTime = GetWorld()->GetTimeSeconds();
		}
	}
}

void ASLBaseAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	UE_LOG(LogAITargeting, Log, TEXT("[%s] Target Forgotten: %s"), *GetName(), *Actor->GetName());
	
	RemoveTarget(Actor);
	
	// 현재 타겟이 잊혀진 경우 재평가
	if (CurrentBestTarget == Actor)
	{
		UpdateTargetEvaluation();
	}
}

void ASLBaseAIController::UpdateTargetEvaluation()
{
	if (!GetPawn())
	{
		return;
	}
	
	AActor* BestTarget = nullptr;
	float BestScore = -FLT_MAX;
	FVector MyLocation = GetPawn()->GetActorLocation();
	
	// 오래된 타겟 정보 제거
	TArray<AActor*> TargetsToRemove;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	for (auto& TargetPair : PotentialTargets)
	{
		// 죽었거나 유효하지 않은 타겟 제거
		if (!IsValid(TargetPair.Key) || !IsActorAlive(TargetPair.Key))
		{
			TargetsToRemove.Add(TargetPair.Key);
			continue;
		}
		
		// 5초 이상 보이지 않은 타겟 제거
		if (CurrentTime - TargetPair.Value.LastSeenTime > 5.0f && TargetPair.Value.LastSeenTime > 0)
		{
			TargetsToRemove.Add(TargetPair.Key);
			continue;
		}
		
		// 타겟 점수 계산
		float Distance = FVector::Dist(MyLocation, TargetPair.Key->GetActorLocation());
		float Score = TargetPair.Value.ThreatLevel - (Distance / 1000.0f);
		
		// 현재 타겟인 경우 보너스 (자주 바뀌지 않도록)
		if (TargetPair.Key == CurrentBestTarget)
		{
			Score += 0.5f;
		}
		
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = TargetPair.Key;
		}
	}
	
	// 오래된 타겟 제거
	for (AActor* Target : TargetsToRemove)
	{
		UE_LOG(LogAITargeting, Log, TEXT("[%s] Removing Old Target: %s"), *GetName(), *Target->GetName());
		PotentialTargets.Remove(Target);
	}
	
	// 새로운 최적 타겟 설정
	if (BestTarget != CurrentBestTarget || !IsActorAlive(CurrentBestTarget))
	{
		AActor* PreviousTarget = CurrentBestTarget;
		CurrentBestTarget = BestTarget;
		
		if (BestTarget)
		{
			UE_LOG(LogAITargeting, Warning, TEXT("[%s] Best Target Changed: %s -> %s (Score: %.2f)"), 
				*GetName(), 
				PreviousTarget ? *PreviousTarget->GetName() : TEXT("None"), 
				*BestTarget->GetName(), 
				BestScore);
		}
		else
		{
			UE_LOG(LogAITargeting, Warning, TEXT("[%s] No Valid Target Found"), *GetName());
		}
		
		if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsObject(FName("TargetActor"), BestTarget);
		}
	}
}

void ASLBaseAIController::AddOrUpdateTarget(AActor* Actor)
{
	if (!Actor || !GetPawn())
	{
		return;
	}
	
	FVector MyLocation = GetPawn()->GetActorLocation();
	float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
	
	if (FTargetInfo* ExistingInfo = PotentialTargets.Find(Actor))
	{
		float OldDistance = ExistingInfo->Distance;
		ExistingInfo->Distance = Distance;
		ExistingInfo->LastSeenTime = 0.0f; // 현재 보이는 상태
		
		UE_LOG(LogAITargeting, Verbose, TEXT("[%s] Target Updated: %s (Distance: %.1f -> %.1f, Threat: %.2f)"), 
			*GetName(), *Actor->GetName(), OldDistance, Distance, ExistingInfo->ThreatLevel);
	}
	else
	{
		FTargetInfo NewInfo;
		NewInfo.Actor = Actor;
		NewInfo.Distance = Distance;
		NewInfo.ThreatLevel = CalculateThreatLevel(Actor);
		NewInfo.LastSeenTime = 0.0f;
		
		PotentialTargets.Add(Actor, NewInfo);
		
		UE_LOG(LogAITargeting, Log, TEXT("[%s] New Target Added: %s (Distance: %.1f, Threat: %.2f)"), 
			*GetName(), *Actor->GetName(), Distance, NewInfo.ThreatLevel);
	}
}

void ASLBaseAIController::RemoveTarget(AActor* Actor)
{
	if (PotentialTargets.Remove(Actor) > 0)
	{
		UE_LOG(LogAITargeting, Log, TEXT("[%s] Target Removed: %s"), *GetName(), *Actor->GetName());
	}
}

float ASLBaseAIController::CalculateThreatLevel(AActor* Actor) const
{
	float ThreatLevel = 1.0f;
	
	// 플레이어인 경우 높은 위협도
	if (Actor->IsA<ASLPlayerCharacterBase>())
	{
		ThreatLevel = 2.0f;
	}
	
	// 거리에 따른 위협도 조정 (가까울수록 위험)
	if (GetPawn())
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
		ThreatLevel += (2000.0f - FMath::Clamp(Distance, 0.0f, 2000.0f)) / 2000.0f;
	}
	
	return ThreatLevel;
}

AActor* ASLBaseAIController::GetBestTarget() const
{
	return CurrentBestTarget;
}

bool ASLBaseAIController::IsActorAlive(AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return false;
	}
	
	// AI 캐릭터인 경우
	if (ASLAIBaseCharacter* AICharacter = Cast<ASLAIBaseCharacter>(Actor))
	{
		return !AICharacter->GetIsDead();
	}
	
	// 플레이어 캐릭터인 경우
	/*if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(Actor))
	{
		return PlayerCharacter->GetCurrentHealth() > 0;
	}*/
	
	return true;
}

void ASLBaseAIController::CleanupDeadTargets()
{
	TArray<AActor*> DeadTargets;
	
	// 죽은 타겟 찾기
	for (auto& TargetPair : PotentialTargets)
	{
		if (!IsActorAlive(TargetPair.Key))
		{
			DeadTargets.Add(TargetPair.Key);
		}
	}
	
	// 죽은 타겟 제거
	for (AActor* DeadTarget : DeadTargets)
	{
		OnTargetDeath(DeadTarget);
	}
}

void ASLBaseAIController::OnTargetDeath(AActor* DeadActor)
{
	if (!DeadActor)
	{
		return;
	}
	
	UE_LOG(LogAITargeting, Warning, TEXT("[%s] Target Died: %s"), *GetName(), *DeadActor->GetName());
	
	// 타겟 리스트에서 제거
	PotentialTargets.Remove(DeadActor);
	
	// 현재 타겟이 죽었다면
	if (CurrentBestTarget == DeadActor)
	{
		UE_LOG(LogAITargeting, Warning, TEXT("[%s] Current Best Target Died, Finding New Target"), *GetName());
		
		CurrentBestTarget = nullptr;
		
		if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			BlackboardComponent->ClearValue(FName("TargetActor"));
		}
		
		// 즉시 새로운 타겟 찾기
		UpdateTargetEvaluation();
	}
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

bool ASLBaseAIController::IsPlayerAttacking() const
{
	ASLPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return false;
	}

	return PlayerCharacter->PrimaryStateTags.HasTag(TAG_Character_Attack);
}

bool ASLBaseAIController::IsPlayerPerformingSpecificAttack(FGameplayTag AttackTag) const
{
	ASLPlayerCharacter* PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter)
	{
		return false;
	}
    
	return PlayerCharacter->PrimaryStateTags.HasTag(AttackTag);
}

ASLPlayerCharacter* ASLBaseAIController::GetPlayerCharacter() const
{
	if (!GetBlackboardComponent())
	{
		return nullptr;
	}
    
	return Cast<ASLPlayerCharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));
}