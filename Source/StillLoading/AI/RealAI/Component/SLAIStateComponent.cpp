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
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogAIStateComponent);

USLAIStateComponent::USLAIStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	CurrentState = EAIBattleState::Idle;
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::BeginPlay()
{
	Super::BeginPlay();
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

	if (IsValid(CachedAIController))
	{
		if (CachedMyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage)
			|| CachedMyCharacter->IsInPrimaryState(TAG_AI_Dead))
		{
			if (CachedMyCharacter->GetLastAnimType() == EHitAnimType::HAT_FallBack)
			{
				CachedAIController->StopMovement();
			}
			return;
		}
	}

	if (IsValid(CachedMyCharacter->BattleManager) && CachedMyCharacter->BattleManager->IsBerserkMode() || bIsBerserkMode)
	{
		SetBerserkMode(DeltaTime);
	}
	else
	{
		PerformEnemyDetection();
 		AActor* DetectedEnemy = LastDetectedEnemy.Get();
  		if (IsValid(DetectedEnemy))
		{
			CombatComponent->SafeLookAtTarget(DetectedEnemy, DeltaTime);
		}
		CombatComponent->HandleEnemyDetection(DetectedEnemy);
	}

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
		if (IsValid(CachedAIController) && IsValid(CachedMyCharacter))
		{
			CachedAIController->StopMovement();
			CachedMyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		break;

	case EAIBattleState::Moving:
		if (IsValid(CachedAIController) && IsValid(CachedMyCharacter))
		{
			CachedMyCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		break;

	case EAIBattleState::Attacking:
		if (IsValid(CachedAIController) && IsValid(CachedMyCharacter))
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
		if (IsValid(CachedAIController))
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

		CombatComponent = MyCharacter->AICombatComp;
		CachedLODComponent = MyCharacter->AILODComp;
	}

	SetComponentTickEnabled(true);
}

void USLAIStateComponent::ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex)
{
	SetCurrentTargetPointIndex(InitialTargetPointIndex);
	RequestNextTargetPoint();
}

void USLAIStateComponent::DeactivateAndReset()
{
	if (IsValid(CachedAIController))
	{
		CachedAIController->StopMovement();
	}

	if (CombatComponent)
	{
		CombatComponent->ReleaseCurrentTargetEngagement();
		CombatComponent->ClearTarget();
	}

	SetState(EAIBattleState::Idle);
	SetComponentTickEnabled(false);
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::SetMovementTarget(FVector NewTargetLocation, float AvailRange)
{
	if (NewTargetLocation.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		LogStateModeStatus(TEXT("유효하지 않은 이동 목표"));
		return;
	}

	if (IsValid(CachedMyCharacter) && IsValid(CachedMyCharacter->AIAttributeComp))
	{
		AvailRange = CachedMyCharacter->AIAttributeComp->GetAbleDistance();
	}

	MovementTargetLocation = NewTargetLocation;

	if (IsValid(CachedAIController))
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

	if (IsValid(MyCharacter->BornSpawner))
	{
		const FVector NextLocation = MyCharacter->BornSpawner->GetNextTargetPointLocation(CurrentTargetPointIndex);

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

void USLAIStateComponent::SetBerserkMode(const float DeltaTime)
{
	if (APawn* PlayerPawn = CachedMyCharacter->BattleManager->GetPrimaryTarget())
	{
		if (IsValid(PlayerPawn) && IsValid(CombatComponent))
		{
			// TODO::추후에 속도 감속 필요 지금은 버서크모드 켜는것만 사용
			if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
			{
				if (auto* MoveComp = Character->GetCharacterMovement())
				{
					if (auto* CharacterMoveComp = Cast<ACharacter>(PlayerPawn)->GetCharacterMovement())
					{
						USkeletalMeshComponent* MeshComp = Character->GetMesh();
						MeshComp->GlobalAnimRateScale = 1.5;
						MoveComp->MaxWalkSpeed = CharacterMoveComp->MaxWalkSpeed + 50.0f;
					}
				}
			}

			CombatComponent->SafeLookAtTarget(PlayerPawn, DeltaTime);
			CombatComponent->HandleEnemyDetection(PlayerPawn);

			const float DistSq = FVector::DistSquared(GetOwner()->GetActorLocation(), PlayerPawn->GetActorLocation());
			float AttackRange = 150.f;
			if (IsValid(CachedMyCharacter) && IsValid(CachedMyCharacter->AIAttributeComp))
			{
				AttackRange = CachedMyCharacter->AIAttributeComp->GetAttackRange();
			}
			const float AttackRangeSq = FMath::Square(AttackRange);

			if (DistSq > AttackRangeSq)
			{
				SetMovementTarget(PlayerPawn->GetActorLocation(), 100.f);
			}
			else
			{
				if (CurrentState != EAIBattleState::Attacking)
				{
					SetState(EAIBattleState::Attacking);
				}
			}
		}
	}
}

FVector USLAIStateComponent::FindSupportPosition(const FVector& TargetLocation, const FVector& MyLocation) const
{
	float AttackRange = 150.0f;

	if (IsValid(CachedMyCharacter) && IsValid(CachedMyCharacter->AIAttributeComp))
	{
		AttackRange = CachedMyCharacter->AIAttributeComp->GetAttackRange();
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
