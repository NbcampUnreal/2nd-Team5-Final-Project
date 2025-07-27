#include "SLAIStateComponent.h"

#include "AIController.h"
#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "Engine/OverlapResult.h"
#include "Navigation/PathFollowingComponent.h"

USLAIStateComponent::USLAIStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;

	CurrentState = EAIBattleState::Idle;
	LastAttackTime = -9999.0f; // 초기화
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USLAIStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !IsValid(GetOwner()))
	{
		return;
	}

	AActor* DetectedEnemy = FindEnemyInDetectionRange();

	if (IsValid(DetectedEnemy))
	{
		SetTarget(DetectedEnemy);
		SetState(EAIBattleState::Attacking);
		GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
	}
	else
	{
		if (IsValid(CurrentTarget.Get()))
		{
			if (!GetWorld()->GetTimerManager().IsTimerActive(TargetClearTimerHandle))
			{
				UE_LOG(LogTemp, Log, TEXT("%s: 타겟 '%s'이(가) 탐지 범위에서 벗어났습니다. %f초 후 타겟 해제 타이머 시작."),
				       *GetOwner()->GetName(), *CurrentTarget->GetName(), TargetLostGracePeriod);
				GetWorld()->GetTimerManager().SetTimer(
					TargetClearTimerHandle,
					this,
					&USLAIStateComponent::ClearTargetInternal,
					TargetLostGracePeriod,
					false
				);
			}
		}
		else
		{
			if (CurrentState == EAIBattleState::Attacking)
			{
				RequestNextTargetPoint();
			}
		}
	}

	switch (CurrentState)
	{
	case EAIBattleState::Idle:
	case EAIBattleState::Moving:
		break;
	case EAIBattleState::Attacking:
		UpdateAttacking(DeltaTime);
		break;
	}
}

void USLAIStateComponent::Initialize()
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		CachedAIController = Cast<AAIController>(OwnerPawn->GetController());
		if (CachedAIController.IsValid())
		{
			if (IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(CachedAIController))
			{
				MyTeamId = OwnerTeamAgent->GetGenericTeamId();
			}

			if (CachedAIController->GetPathFollowingComponent())
			{
				CachedAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
					this, &USLAIStateComponent::OnMoveCompleted);
			}
		}
	}
}

void USLAIStateComponent::ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex)
{
	if (CurrentState != EAIBattleState::Idle)
	{
		return;
	}

	SetCurrentTargetPointIndex(InitialTargetPointIndex);
	RequestNextTargetPoint();
}

void USLAIStateComponent::DeactivateAndReset()
{
	if (CachedAIController.IsValid())
	{
		CachedAIController->StopMovement();
	}

	ClearTarget();

	SetState(EAIBattleState::Idle);

	LastAttackTime = -9999.0f;
	CurrentTargetPointIndex = 0;
}

AActor* USLAIStateComponent::FindEnemyInDetectionRange() const
{
	TArray<FOverlapResult> OverlappingResults;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	GetWorld()->OverlapMultiByChannel(
		OverlappingResults,
		OwnerLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(DetectionRange),
		FCollisionQueryParams(SCENE_QUERY_STAT(AIAOD_Detection), false)
	);

	AActor* NearestEnemy = nullptr;
	float MinDistSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : OverlappingResults)
	{
		AActor* OverlappedActor = Overlap.GetActor();
		if (!IsValid(OverlappedActor) || OverlappedActor == GetOwner()) continue;

		const APawn* TargetPawn = Cast<APawn>(OverlappedActor);
		if (!TargetPawn) return nullptr;

		AController* TargetController = TargetPawn->GetController();
		if (!TargetController) return nullptr;

		bool bTargetIsPlayer = TargetController->IsPlayerController();

		IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetController);
		if (!TargetTeamAgent) return nullptr;

		const FGenericTeamId TargetTeamId = TargetTeamAgent->GetGenericTeamId();

		if (TargetTeamId == FGenericTeamId::NoTeam)
		{
			continue;
		}

		const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
		if (!MyCharacter) return nullptr;

		if (MyCharacter->BattleManager &&
			MyCharacter->BattleManager->AreEnemies(MyTeamId, TargetTeamAgent->GetGenericTeamId(), bTargetIsPlayer))
		{
			float DistSq = FVector::DistSquared(OwnerLocation, OverlappedActor->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				NearestEnemy = OverlappedActor;
			}
		}
	}

	return NearestEnemy;
}

void USLAIStateComponent::SetState(EAIBattleState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		OnEnterState(NewState);
		OnStateChanged.Broadcast(NewState);
		UE_LOG(LogTemp, Log, TEXT("%s: 상태 변경 -> %s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(NewState));
	}
}

// 상태 진입 로직
void USLAIStateComponent::OnEnterState(EAIBattleState NewState)
{
	switch (NewState)
	{
	case EAIBattleState::Idle:
		if (CachedAIController.IsValid())
		{
			CachedAIController->StopMovement();
		}
		RequestNextTargetPoint();
		break;
	case EAIBattleState::Moving:
		break;
	case EAIBattleState::Attacking:
		break;
	}
}

void USLAIStateComponent::SetTarget(AActor* NewTarget)
{
	if (CurrentTarget != NewTarget)
	{
		CurrentTarget = NewTarget;
		OnTargetChanged.Broadcast(NewTarget);
		UE_LOG(LogTemp, Log, TEXT("%s: 타겟 설정 -> %s"), *GetOwner()->GetName(),
		       IsValid(NewTarget) ? *NewTarget->GetName() : TEXT("없음"));
	}
}

void USLAIStateComponent::SetMovementTarget(FVector NewTargetLocation)
{
	if (NewTargetLocation.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: 유효하지 않은 (0,0,0에 가까운) 이동 목표가 전달되었습니다. 이동을 건너뜜."), *GetOwner()->GetName());
		return;
	}

	MovementTargetLocation = NewTargetLocation;

	if (CachedAIController.IsValid())
	{
		FAIRequestID RequestID = CachedAIController->MoveToLocation(MovementTargetLocation);

		if (RequestID == FAIRequestID::InvalidRequest)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: MoveToLocation 명령이 실패했습니다 (InvalidRequest). 목표: %s"),
			       *GetOwner()->GetName(), *MovementTargetLocation.ToString());
			SetState(EAIBattleState::Idle);
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("%s: 새로운 이동 목표 설정 및 이동 시작: %s"), *GetOwner()->GetName(),
		       *MovementTargetLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: AI 컨트롤러가 유효하지 않아 이동 목표를 설정할 수 없습니다."), *GetOwner()->GetName());
		SetState(EAIBattleState::Idle);
	}
}

void USLAIStateComponent::ClearTarget()
{
	SetTarget(nullptr);
	GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
}

bool USLAIStateComponent::CanAttack() const
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return false;
	}

	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());
	return Distance <= AttackRange;
}

void USLAIStateComponent::PerformAttack()
{
	if (CanAttack())
	{
		LastAttackTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogTemp, Log, TEXT("%s: %s 공격!"), *GetOwner()->GetName(), *CurrentTarget->GetName());

		
	}
}

void USLAIStateComponent::UpdateAttacking(float DeltaTime)
{
	if (!CurrentTarget.IsValid())
	{
		ClearTarget();
		RequestNextTargetPoint();
		return;
	}

	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());

	if (Distance > AttackRange * 1.2f)
	{
		SetMovementTarget(CurrentTarget->GetActorLocation());
		return;
	}

	PerformAttack();
}

void USLAIStateComponent::SetCurrentTargetPointIndex(int32 NewIndex)
{
	CurrentTargetPointIndex = NewIndex;
}

void USLAIStateComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (CurrentState == EAIBattleState::Moving)
	{
		GetWorld()->GetTimerManager().SetTimer(
			MovementCompletionTimerHandle,
			this,
			&USLAIStateComponent::RequestNextTargetPoint,
			0.1f,
			false
		);
	}
}

void USLAIStateComponent::RequestNextTargetPoint()
{
	const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
	if (!MyCharacter) return;

	if (MyCharacter->BattleManager && SourceSpawner.IsValid())
	{
		const FVector NextLocation = MyCharacter->BattleManager->GetNextTargetPointLocationForSpawner(
			SourceSpawner.Get(), CurrentTargetPointIndex);

		if (!NextLocation.IsNearlyZero())
		{
			FVector CurrentLocation = GetOwner()->GetActorLocation();
			float DistanceToTarget = FVector::Dist(CurrentLocation, NextLocation);

			if (DistanceToTarget > 300.0f)
			{
				SetMovementTarget(NextLocation);
				SetState(EAIBattleState::Moving);
			}
			else
			{
				SetState(EAIBattleState::Idle);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: 스포너 '%s'에 더 이상 유효한 타겟 포인트가 없습니다. 이동 중지."),
			       *GetOwner()->GetName(), *SourceSpawner->GetName());
			SetState(EAIBattleState::Idle);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: CachedBattleManager 또는 SourceSpawner가 유효하지 않습니다. 다음 타겟 포인트를 요청할 수 없습니다."),
		       *GetOwner()->GetName());
		SetState(EAIBattleState::Idle);
	}
}

void USLAIStateComponent::ClearTargetInternal()
{
	if (!IsValid(CurrentTarget.Get()))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s: 타겟이 탐지 범위에서 벗어난 후 %f초가 지나 타겟을 해제합니다."),
	       *GetOwner()->GetName(), TargetLostGracePeriod);
	SetTarget(nullptr);
	RequestNextTargetPoint();
}
