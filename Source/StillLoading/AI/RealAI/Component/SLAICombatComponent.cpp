#include "SLAICombatComponent.h"

#include "SLAIAttributeComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "SLAIStateComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogAICombatComponent);

USLAICombatComponent::USLAICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	LastAttackTime = -9999.0f;
}

void USLAICombatComponent::BeginPlay()
{
	Super::BeginPlay();

	StateComponent = GetOwner()->FindComponentByClass<USLAIStateComponent>();

	if (UCharacterMovementComponent* MovementComponent = GetOwner()->FindComponentByClass<
		UCharacterMovementComponent>())
	{
		OriginalSpeed = MovementComponent->MaxWalkSpeed;
	}

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		CachedMyCharacter = MyCharacter;
		if (MyCharacter)
		{
			CachedAIController = Cast<AAIController>(MyCharacter->GetController());
		}
	}
}

void USLAICombatComponent::SafeLookAtTarget(AActor* Target, float DeltaTime)
{
	if (!Target || !CachedMyCharacter) return;

	const FVector ToTarget = Target->GetActorLocation() - CachedMyCharacter->GetActorLocation();
	if (ToTarget.IsNearlyZero()) return;

	const FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	const FRotator CurrentRotation = CachedMyCharacter->GetActorRotation();

	constexpr float RotationSpeed = 3.0f;
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

	CachedMyCharacter->SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f)); // Pitch, Roll은 0으로
}

void USLAICombatComponent::UpdateAttacking(float DeltaTime)
{
	if (!IsValid(CurrentTarget.TargetActor))
	{
		if (StateComponent && StateComponent->CachedAIController.IsValid())
		{
			StateComponent->CachedAIController->ClearFocus(EAIFocusPriority::Gameplay);
		}

		if (StateComponent)
		{
			StateComponent->SetState(EAIBattleState::Idle);
		}
		return;
	}

	if (bIsRetreating || bIsOrbiting)
	{
		return;
	}

	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget.TargetActor->GetActorLocation());
	if (Distance > AttackRange)
	{
		if (StateComponent && !bIsOrbiting && !bIsRetreating)
		{
			if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
			{
				if (MyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage) || MyCharacter->IsInPrimaryState(TAG_AI_Dead)) return;
			}
			
			StateComponent->SetMovementTarget(CurrentTarget.TargetActor->GetActorLocation(), 50);
		}
	}
	else if (CanAttack())
	{
		PerformAttack(DeltaTime);
	}
}

void USLAICombatComponent::UpdateSupporting(float DeltaTime)
{
	bIsSupporting = true;
}

// StateComponent 에서 실행
AActor* USLAICombatComponent::FindEnemyInDetectionRange() const
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
	float MinDistanceSq = FLT_MAX;

	for (const FOverlapResult& Overlap : OverlappingResults)
	{
		AActor* OverlappedActor = Overlap.GetActor();

		if (!IsValidEnemy(OverlappedActor))
			continue;

		const float DistanceSq = FVector::DistSquared(OwnerLocation, OverlappedActor->GetActorLocation());
		if (DistanceSq < MinDistanceSq)
		{
			MinDistanceSq = DistanceSq;
			NearestEnemy = OverlappedActor;
		}
	}

	return NearestEnemy;
}

// StateComponent 에서 실행
void USLAICombatComponent::HandleEnemyDetection(AActor* DetectedEnemy)
{
	if (bIsOrbiting || bIsRetreating) return;

	if (IsValid(DetectedEnemy))
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float RandomHoldingTime = FMath::RandRange(10.0f, 15.0f);
		if (IsValid(CurrentTarget.TargetActor) && CurrentTime - CurrentTarget.HoldingTime > RandomHoldingTime)
		{
			ClearTarget();
			return;
		}

		if (!IsValid(CurrentTarget.TargetActor) || CurrentTarget.TargetActor != DetectedEnemy)
		{
			ASLBattleManager* BattleManager = CachedMyCharacter->BattleManager;
			if (!BattleManager) return;

			if (!BattleManager->PlayerOnly())
			{
				if (RequestEngagementPermission(DetectedEnemy))
				{
					LogCombatModeStatus(FString::Printf(TEXT("전투 권환 흭득 [%s]"), *GetOwner()->GetName()));
					SetTarget(DetectedEnemy);
					if (StateComponent)
					{
						StateComponent->SetState(EAIBattleState::Attacking);
					}
				}
			}
			else
			{
				SetTarget(DetectedEnemy);
				if (StateComponent)
				{
					StateComponent->SetState(EAIBattleState::Attacking);
				}
			}
		}

		if (GetWorld()->GetTimerManager().IsTimerActive(TargetClearTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
		}
	}
	else
	{
		HandleNoEnemyDetected();
	}
}

void USLAICombatComponent::HandleNoEnemyDetected()
{
	if (IsValid(CurrentTarget.TargetActor))
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TargetClearTimerHandle))
		{
			LogCombatModeStatus(FString::Printf(TEXT("타겟 '%s' 탐지 범위 이탈 - %.1f초 후 타겟 해제"),
			                                    *CurrentTarget.TargetActor->GetName(), TargetLostGracePeriod));

			GetWorld()->GetTimerManager().SetTimer(
				TargetClearTimerHandle,
				this,
				&USLAICombatComponent::ClearTargetInternal,
				TargetLostGracePeriod,
				false
			);
		}
	}
}

void USLAICombatComponent::SetTarget(AActor* NewTarget)
{
	if (CurrentTarget.TargetActor != NewTarget)
	{
		if (IsValid(CurrentTarget.TargetActor) && CurrentTarget.TargetActor != NewTarget)
		{
			ReleaseCurrentTargetEngagement();
		}

		CurrentTarget.TargetActor = NewTarget;
		CurrentTarget.HoldingTime = GetWorld()->GetTimeSeconds();
	}
}

void USLAICombatComponent::ClearTarget()
{
	SetTarget(nullptr);
	GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
	if (StateComponent)
	{
		StateComponent->RequestNextTargetPoint();
	}
}

bool USLAICombatComponent::CanAttack() const
{
	if (!IsValid(CurrentTarget.TargetActor)) return false;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return false;
	}

	float AvailDistance = 150.0f;
	if (IsValid(CachedMyCharacter))
	{
		if (CachedMyCharacter->AIAttributeComp->AIUnitType == EAIUnitType::Ranger)
		{
			AvailDistance = 500.0f;
		}
	}

	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget.TargetActor->GetActorLocation());
	return Distance <= AvailDistance;
}

void USLAICombatComponent::PerformAttack(float DeltaTime)
{
	LastAttackTime = GetWorld()->GetTimeSeconds();
	LogCombatModeStatus(FString::Printf(TEXT("%s 공격!"), *CurrentTarget.TargetActor->GetName()));

	SafeLookAtTarget(CurrentTarget.TargetActor, DeltaTime);
	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		MyCharacter->PlayAttackAnim();
	}

	TryScheduleRetreat();
}

void USLAICombatComponent::StartRetreating()
{
	if (!bIsRetreating)
	{
		if (StateComponent && StateComponent->CachedAIController.IsValid())
		{
			StateComponent->CachedAIController->StopMovement();
		}

		bIsRetreating = true;
		RetreatDistance = FMath::RandRange(200.0f, 400.0f);

		RetreatFromTarget();

		GetWorld()->GetTimerManager().SetTimer(
			RetreatTimerHandle,
			this,
			&USLAICombatComponent::StopRetreating,
			1.5f,
			false
		);
	}
}

void USLAICombatComponent::RetreatFromTarget()
{
	AActor* MyActor = GetOwner();
	if (!IsValid(CurrentTarget.TargetActor) || !MyActor)
	{
		StopRetreating();
		return;
	}

	AAIController* MyController = Cast<AAIController>(MyActor->GetInstigatorController());
	if (!MyController)
	{
		UE_LOG(LogTemp, Error, TEXT("RetreatFromTarget: AIController is invalid!"));
		StopRetreating();
		return;
	}

	const FVector MyLocation = MyActor->GetActorLocation();
	const FVector TargetLocation = CurrentTarget.TargetActor->GetActorLocation();
	const FVector RetreatDirection = (MyLocation - TargetLocation).GetSafeNormal();

	if (RetreatDirection.IsNearlyZero())
	{
		StopRetreating();
		return;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		StopRetreating();
		return;
	}

	FNavLocation NavigableRetreatLocation;
	bool bFoundPoint = false;

	for (int32 i = 0; i < 10; ++i)
	{
		FVector RandomOffset = FMath::VRand() * FMath::FRandRange(RetreatDistance * 0.5f, RetreatDistance);
		FVector TestPoint = MyLocation + RetreatDirection * RetreatDistance + RandomOffset;

		if (NavSys->GetRandomReachablePointInRadius(TestPoint, 200.0f, NavigableRetreatLocation))
		{
			bFoundPoint = true;
			break;
		}
	}

	if (!bFoundPoint)
	{
		bFoundPoint = NavSys->GetRandomReachablePointInRadius(MyLocation, RetreatDistance, NavigableRetreatLocation);
	}

	if (bFoundPoint)
	{
		//DrawDebugSphere(GetWorld(), NavigableRetreatLocation.Location, 50.f, 12, FColor::Green, false, 3.0f);

		if (auto* MoveComp = Cast<ACharacter>(MyActor)->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = 300.0f;
		}

		MyController->SetFocus(CurrentTarget.TargetActor);
		MyController->MoveToLocation(NavigableRetreatLocation.Location);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("최종 후퇴 지점 탐색 실패! 주변에 NavMesh가 충분하지 않을 수 있습니다."));
		StopRetreating();
	}
}

void USLAICombatComponent::StartRandomTurn()
{
	bIsOrbiting = true;
	OrbitDirection = FMath::RandBool() ? 1.0f : -1.0f;

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = 300.0f;
			DefaultBrakingDeceleration = MoveComp->BrakingDecelerationWalking;
			MoveComp->BrakingDecelerationWalking = 0.f;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		OrbitUpdateTimerHandle,
		this,
		&USLAICombatComponent::UpdateOrbiting,
		0.3f,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		EndOrbitTimerHandle,
		this,
		&USLAICombatComponent::FinishRandomTurn,
		FMath::RandRange(2.0f, 3.0f),
		false
	);
}

void USLAICombatComponent::FinishRandomTurn()
{
	if(!bIsOrbiting) return;
	
	bIsOrbiting = false;
	GetWorld()->GetTimerManager().ClearTimer(OrbitUpdateTimerHandle);

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = OriginalSpeed;
			MoveComp->BrakingDecelerationWalking = DefaultBrakingDeceleration;
		}

		if (AAIController* MyController = MyCharacter->GetController<AAIController>())
		{
			MyController->StopMovement();
		}
	}
}

void USLAICombatComponent::UpdateOrbiting()
{
	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (MyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage) || MyCharacter->IsInPrimaryState(TAG_AI_Dead)) return;
	}
	
	if (!bIsOrbiting || !IsValid(CurrentTarget.TargetActor))
	{
		FinishRandomTurn();
		return;
	}

	ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!MyCharacter) return;

	AAIController* MyController = MyCharacter->GetController<AAIController>();
	if (!MyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateOrbiting: AIController가 없습니다!"));
		return;
	}

	const FVector MyLocation = MyCharacter->GetActorLocation();
	const FVector TargetLocation = CurrentTarget.TargetActor->GetActorLocation();
	const float OrbitRadius = 400.0f;

	FVector DirectionFromTarget = MyLocation - TargetLocation;
	DirectionFromTarget.Z = 0;

	if (DirectionFromTarget.IsNearlyZero())
	{
		DirectionFromTarget = MyCharacter->GetActorForwardVector();
	}

	const FVector NormalizedDirection = DirectionFromTarget.GetSafeNormal();

	const float AngleStep = 25.0f;
	const FVector RotatedDirection = NormalizedDirection.RotateAngleAxis(AngleStep * OrbitDirection, FVector::UpVector);
	const FVector NextOrbitPoint = TargetLocation + RotatedDirection.GetSafeNormal() * OrbitRadius;

	MyController->MoveToLocation(
		NextOrbitPoint,
		-1.0f, // AcceptanceRadius: 목표에 얼마나 가까워져야 성공으로 간주할지 (-1은 기본값 사용)
		true, // bStopOnOverlap
		true // bUsePathfinding
	);
}

void USLAICombatComponent::StopRetreating()
{
	if (!bIsRetreating) return;
	bIsRetreating = false;

	StartRandomTurn();

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = OriginalSpeed;
		}
	}
}

void USLAICombatComponent::TryScheduleRetreat()
{
	ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!MyCharacter) return;

	if (MyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage))
	{
		GetWorld()->GetTimerManager().SetTimer(
			BeginRetreatTimerHandle,
			this,
			&USLAICombatComponent::TryScheduleRetreat,
			0.2f,
			false
		);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			BeginRetreatTimerHandle,
			this,
			&USLAICombatComponent::StartRetreating,
			1.0f,
			false
		);
	}
}

bool USLAICombatComponent::RequestEngagementPermission(AActor* TargetActor)
{
	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (MyCharacter->BattleManager)
		{
			return MyCharacter->BattleManager->RequestEngagementPermission(GetOwner(), TargetActor);
		}
	}
	return false;
}

void USLAICombatComponent::ReleaseCurrentTargetEngagement()
{
	if (!IsValid(CurrentTarget.TargetActor)) return;

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (IsValid(MyCharacter->BattleManager))
		{
			MyCharacter->BattleManager->ReleaseEngagementPermission(GetOwner(), CurrentTarget.TargetActor);
		}
	}
}

bool USLAICombatComponent::IsValidEnemy(AActor* Actor) const
{
	if (!IsValid(Actor) || Actor == GetOwner())
		return false;

	const APawn* TargetPawn = Cast<APawn>(Actor);
	if (!TargetPawn)
		return false;

	AController* TargetController = TargetPawn->GetController();
	if (!TargetController)
		return false;

	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetController);
	if (!TargetTeamAgent)
		return false;

	const FGenericTeamId TargetTeamId = TargetTeamAgent->GetGenericTeamId();
	if (TargetTeamId == FGenericTeamId::NoTeam)
		return false;

	const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
	if (!MyCharacter || !MyCharacter->BattleManager)
		return false;

	bool bTargetIsPlayer = TargetController->IsPlayerController();
	return MyCharacter->BattleManager->AreEnemies(StateComponent->MyTeamId, TargetTeamId, bTargetIsPlayer);
}

void USLAICombatComponent::ClearTargetInternal()
{
	if (!IsValid(CurrentTarget.TargetActor))
	{
		return;
	}

	LogCombatModeStatus(FString::Printf(TEXT("타겟이 탐지 범위에서 벗어난 후 %.1f초가 지나 타겟을 해제합니다."),
	                                    TargetLostGracePeriod));

	ClearTarget();

	if (StateComponent)
	{
		StateComponent->RequestNextTargetPoint();
	}
}

void USLAICombatComponent::LogCombatModeStatus(const FString& Message) const
{
	UE_LOG(LogAICombatComponent, Log, TEXT("%s [전투모드]: %s"),
	       GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), *Message);
}
