#include "SLAIStateComponent.h"

#include "SLAIAttributeComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "SLAICombatComponent.h"
#include "SLAILODComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogAIStateComponent);

USLAIStateComponent::USLAIStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	CurrentState = EAIBattleState::Idle;
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::BeginPlay()
{
	Super::BeginPlay();

	const float InitialDelay = FMath::FRandRange(0.0f, 0.5f);

	GetWorld()->GetTimerManager().SetTimer(
		DetectionTimerHandle,
		this,
		&USLAIStateComponent::PerformEnemyDetection,
		1.0f,
		true,
		InitialDelay);

	Initialize();
}

void USLAIStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CachedMyCharacter))
	{
		return;
	}

	if (CachedMyCharacter)
	{
		if (CachedMyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage) || CachedMyCharacter->
			IsInPrimaryState(TAG_AI_Dead)) return;
	}

	AActor* DetectedEnemy = LastDetectedEnemy.Get();
	if (IsValid(DetectedEnemy))
	{
		CombatComponent->SafeLookAtTarget(DetectedEnemy, DeltaTime);
	}
	CombatComponent->HandleEnemyDetection(DetectedEnemy);

	UpdateCurrentState(DeltaTime);
}

void USLAIStateComponent::PerformEnemyDetection()
{
	if (CombatComponent)
	{
		LastDetectedEnemy = CombatComponent->FindEnemyInDetectionRange();
	}
}

void USLAIStateComponent::UpdateCurrentState(float DeltaTime)
{
	if (IsValid(CachedMyCharacter) && IsValid(CachedMyCharacter->BattleManager) && IsValid(CachedLODComponent))
	{
		const EAILODLevel CurrentLOD = CachedLODComponent->GetCurrentLODLevel();
		if (CurrentLOD == EAILODLevel::Max)
		{
			const auto& UnitMap = CachedMyCharacter->BattleManager->GetUnitIndexMap();
			if (const int32* MyIndexPtr = UnitMap.Find(GetOwner()))
			{
				const FVector TargetLocation = CachedMyCharacter->BattleManager->GetUnitTargetLocations()[*MyIndexPtr];
				if (!TargetLocation.IsNearlyZero())
				{
					if (CurrentState != EAIBattleState::Attacking)
					{
						SetMovementTarget(TargetLocation, 100.f);
						return;
					}
				}
			}
		}
	}
	
	switch (CurrentState)
	{
	case EAIBattleState::Idle:
		break;
	case EAIBattleState::Moving:
		break;

	case EAIBattleState::Attacking:
		if (CombatComponent)
		{
			CombatComponent->UpdateAttacking(DeltaTime);
		}
		break;

	case EAIBattleState::FakeMoving:
		if (CombatComponent)
		{
			CombatComponent->UpdateFakeMovement(DeltaTime);
		}
		break;

	default:
		LogStateModeStatus(TEXT("알 수 없는 AI 상태"));
		break;
	}
}

void USLAIStateComponent::SetState(EAIBattleState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		OnEnterState(NewState);
		OnStateChanged.Broadcast(NewState);
		//LogStateModeStatus(FString::Printf(TEXT("상태 변경 -> %s"), *UEnum::GetValueAsString(NewState)));
	}
}

void USLAIStateComponent::OnEnterState(EAIBattleState NewState)
{
	switch (NewState)
	{
	case EAIBattleState::Idle:
		if (CachedAIController.IsValid() && IsValid(CachedMyCharacter))
		{
			CachedAIController->StopMovement();
			CachedMyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		break;

	case EAIBattleState::Moving:
		if (CachedAIController.IsValid() && IsValid(CachedMyCharacter))
		{
			CachedMyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		break;

	case EAIBattleState::Attacking:
		if (CachedAIController.IsValid() && IsValid(CachedMyCharacter))
		{
			CachedAIController->StopMovement();
			CachedMyCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		break;
	default: ;
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

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		CachedMyCharacter = MyCharacter;
	}

	CombatComponent = GetOwner()->FindComponentByClass<USLAICombatComponent>();
	CachedLODComponent = GetOwner()->FindComponentByClass<USLAILODComponent>();
}

void USLAIStateComponent::ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex)
{
	SetCurrentTargetPointIndex(InitialTargetPointIndex);
	RequestNextTargetPoint();
}

void USLAIStateComponent::DeactivateAndReset()
{
	if (CachedAIController.IsValid())
	{
		CachedAIController->StopMovement();
	}

	if (CombatComponent)
	{
		CombatComponent->ReleaseCurrentTargetEngagement();
		CombatComponent->ClearTarget();
	}

	SetState(EAIBattleState::Idle);
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::SetMovementTarget(FVector NewTargetLocation, float AvailRange)
{
	if (NewTargetLocation.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		LogStateModeStatus(TEXT("유효하지 않은 이동 목표"));
		return;
	}

	MovementTargetLocation = NewTargetLocation;

	if (CachedAIController.IsValid())
	{
		FAIRequestID RequestID = CachedAIController->MoveToLocation(MovementTargetLocation, AvailRange);
		if (CurrentState != EAIBattleState::Attacking)
		{
			SetState(EAIBattleState::Moving);
		}

		if (RequestID == FAIRequestID::InvalidRequest)
		{
			LogStateModeStatus(TEXT("MoveToLocation 실패"));
			SetState(EAIBattleState::Idle);
		}
	}
	else
	{
		LogStateModeStatus(TEXT("AI 컨트롤러가 유효하지 않음"));
		SetState(EAIBattleState::Idle);
	}
}

void USLAIStateComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		if (IsValid(CachedMyCharacter) && IsValid(CachedMyCharacter->BattleManager) && IsValid(CachedLODComponent))
		{
			if (CachedLODComponent->GetCurrentLODLevel() == EAILODLevel::Max)
			{
				SetState(EAIBattleState::Attacking);
				return;
			}
		}
		
		CurrentTargetPointIndex++;

		float RandRequestRange = FMath::RandRange(0.5f, 1.0f);

		GetWorld()->GetTimerManager().SetTimer(
			MovementCompletionTimerHandle,
			this,
			&USLAIStateComponent::RequestNextTargetPoint,
			RandRequestRange,
			false
		);
	}
}

void USLAIStateComponent::RequestNextTargetPoint()
{
	const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
	if (!MyCharacter) return;

	if (IsValid(MyCharacter->BattleManager) && IsValid(MyCharacter->BornSpawner))
	{
		const FVector NextLocation = MyCharacter->BattleManager->GetNextTargetPointLocationForSpawner(
			MyCharacter->BornSpawner, CurrentTargetPointIndex);

		if (!NextLocation.IsNearlyZero())
		{
			FVector CurrentLocation = GetOwner()->GetActorLocation();
			float DistanceToTarget = FVector::Dist(CurrentLocation, NextLocation);

			if (DistanceToTarget > 300.0f)
			{
				SetMovementTarget(NextLocation);
			}
		}
	}
	else
	{
		LogStateModeStatus(TEXT("BattleManager 또는 Spawner가 유효하지 않음"));
		SetState(EAIBattleState::Idle);
	}
}

void USLAIStateComponent::SetCurrentTargetPointIndex(int32 NewIndex)
{
	CurrentTargetPointIndex = NewIndex;
}

// 서포트 모드 관련
void USLAIStateComponent::StartSupportMovement(AActor* TargetToSupport)
{
	if (!IsValid(TargetToSupport)) return;

	SupportTargetActor = TargetToSupport;
	bIsSupportMoving = true;

	const FVector TargetLocation = TargetToSupport->GetActorLocation();
	const FVector MyLocation = GetOwner()->GetActorLocation();

	const FVector SupportPosition = FindSupportPosition(TargetLocation, MyLocation);

	if (!SupportPosition.IsZero())
	{
		SetMovementTarget(SupportPosition);

		LogStateModeStatus(FString::Printf(TEXT("지원 이동 시작 -> %s"), *TargetToSupport->GetName()));
	}
}

FVector USLAIStateComponent::FindSupportPosition(const FVector& TargetLocation, const FVector& MyLocation) const
{
	float AttackRange = 150.0f;

	if (const ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (const USLAIAttributeComponent* AttributeComp = MyCharacter->AIAttributeComp)
		{
			AttackRange = AttributeComp->AttackRange;
		}
	}

	for (int32 i = 0; i < 12; ++i)
	{
		float Angle = (360.0f / 12.0f) * i;
		const float RadianAngle = FMath::DegreesToRadians(Angle);

		FVector TestPosition = TargetLocation + FVector(
			FMath::Cos(RadianAngle) * (AttackRange * 0.8f),
			FMath::Sin(RadianAngle) * (AttackRange * 0.8f),
			0.0f
		);

		FNavLocation NavLocation;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (NavSys && NavSys->GetRandomReachablePointInRadius(TestPosition, 100.0f, NavLocation))
		{
			return NavLocation.Location;
		}
	}

	return FVector::ZeroVector;
}

void USLAIStateComponent::LogStateModeStatus(const FString& Message) const
{
	UE_LOG(LogAIStateComponent, Log, TEXT("%s [상태모드]: %s"),
	       GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), *Message);
}
