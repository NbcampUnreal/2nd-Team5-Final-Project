#include "SLAICombatComponent.h"

#include "AIAttributeComponent.h"
#include "AIController.h"
#include "SLAIStateComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogAICombatComponent);

USLAICombatComponent::USLAICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.TickInterval = 0.2f;
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

	if (CachedMyCharacter)
	{
		CachedMyCharacter->bUseControllerRotationYaw = false;
        
		if (UCharacterMovementComponent* MovementComp = CachedMyCharacter->GetCharacterMovement())
		{
			MovementComp->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
			MovementComp->bOrientRotationToMovement = true;
		}
	}
}

void USLAICombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CachedMyCharacter))
	{
		CachedMyCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
		return;
	}
	
	if (CachedMyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage) || CachedMyCharacter->IsInPrimaryState(TAG_AI_Dead)) return;

	SafeLookAtTarget(CurrentTarget.TargetActor, DeltaTime);

	// 후퇴 로직
	if (bIsRetreating)
	{
		RetreatFromTarget(DeltaTime);
	}

	if (bIsOrbiting)
	{
		UpdateOrbiting(DeltaTime);
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
			StateComponent->RequestNextTargetPoint();
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
			StateComponent->SetMovementTarget(CurrentTarget.TargetActor->GetActorLocation());
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
			float AvailDistance = 300.0f;
			if (IsValid(CachedMyCharacter))
			{
				if (CachedMyCharacter->AIAttributeComp->AIUnitType == EAIUnitType::Ranger)
				{
					AvailDistance = 800.0f;
				}
			}
		
			const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), DetectedEnemy->GetActorLocation());
			if (Distance < AvailDistance)
			{
				if (RequestEngagementPermission(DetectedEnemy))
				{
					LogCombatModeStatus(FString::Printf(TEXT("전투 권환 흭득 [%s]"), *GetOwner()->GetName()));
					SetTarget(DetectedEnemy);
					if (StateComponent)
					{
						StateComponent->SetState(EAIBattleState::Attacking);
					}

					SetComponentTickEnabled(true);
					bIsSupporting = false;
				}
				else
				{
					LogCombatModeStatus(FString::Printf(TEXT("서포팅 모드 진입 [%s]"), *GetOwner()->GetName()));
					StateComponent->SetState(EAIBattleState::SupportOrIdle);
					SetTarget(DetectedEnemy);
					bIsSupporting = true;
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
	else if (StateComponent && StateComponent->GetCurrentState() == EAIBattleState::Attacking)
	{
		StateComponent->RequestNextTargetPoint();
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

		LogCombatModeStatus(FString::Printf(TEXT("타겟 설정 -> %s"),
		                                    IsValid(NewTarget) ? *NewTarget->GetName() : TEXT("없음")));

		SetComponentTickEnabled(IsValid(NewTarget));
	}
}

void USLAICombatComponent::ClearTarget()
{
	SetTarget(nullptr);
	GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
}

bool USLAICombatComponent::CanAttack() const
{
	if (!IsValid(CurrentTarget.TargetActor)) return false;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return false;
	}

	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget.TargetActor->GetActorLocation());
	return Distance <= AttackRange;
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

	StartRetreating();
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
		SetComponentTickEnabled(true);
		RetreatDistance = FMath::RandRange(200.0f, 400.0f);
	}
}

void USLAICombatComponent::RetreatFromTarget(float DeltaTime)
{
	if (!IsValid(CurrentTarget.TargetActor)) return;
	if (bIsOrbiting) return;

	const FVector MyLocation = CachedMyCharacter->GetActorLocation();
	const FVector TargetLocation = CurrentTarget.TargetActor->GetActorLocation();
	const FVector RetreatDirection = (MyLocation - TargetLocation).GetSafeNormal();
	const float CurrentDistance = FVector::Dist(MyLocation, TargetLocation);

	if (CurrentDistance >= RetreatDistance)
	{
		// 대기 상태 진입
		if (StateComponent && StateComponent->CachedAIController.IsValid())
		{
			StateComponent->CachedAIController->StopMovement();
		}
		
		StartRandomTurn();
		StopRetreating();
		RetreatDistance = 0;
		return;
	}

	/*
	const FRotator LookAtRot = FRotationMatrix::MakeFromX(TargetLocation - MyLocation).Rotator();
	CachedMyCharacter->SetActorRotation(LookAtRot);
	*/

	if (UCharacterMovementComponent* MoveComp = CachedMyCharacter->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 200.0f;
	}

	constexpr float RetreatSpeedScale = 0.5f;
	CachedMyCharacter->AddMovementInput(RetreatDirection, RetreatSpeedScale);
}

void USLAICombatComponent::UpdateOrbiting(float DeltaTime) const
{
	if (!bIsOrbiting || !IsValid(CurrentTarget.TargetActor)) return;

	ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!MyCharacter) return;

	const FVector MyLocation = MyCharacter->GetActorLocation();
	const FVector TargetLocation = CurrentTarget.TargetActor->GetActorLocation();
	const FVector DirectionFromTarget = (MyLocation - TargetLocation).GetSafeNormal();

	if (DirectionFromTarget.IsNearlyZero())
	{
		return;
	}
	
	const FVector TangentDirection = FVector(-DirectionFromTarget.Y * OrbitDirection,
	                                         DirectionFromTarget.X * OrbitDirection,
	                                         0.0f);

	/*
	const FRotator LookAtRot = FRotationMatrix::MakeFromX(TargetLocation - MyLocation).Rotator();
	MyCharacter->SetActorRotation(LookAtRot);
	*/

	constexpr float OrbitSpeed = 1.0f;
	MyCharacter->AddMovementInput(TangentDirection, OrbitSpeed);
}

void USLAICombatComponent::StartRandomTurn()
{
	bIsOrbiting = true;
	OrbitDirection = FMath::RandBool() ? 1.0f : -1.0f;

	GetWorld()->GetTimerManager().SetTimer(
		RandomTurnTimerHandle,
		this,
		&USLAICombatComponent::FinishRandomTurn,
		FMath::RandRange(0.5f, 2.0f),
		false
	);
}

void USLAICombatComponent::FinishRandomTurn()
{
	bIsOrbiting = false;
	SetComponentTickEnabled(false);
}

void USLAICombatComponent::StopRetreating()
{
	if (!bIsRetreating) return;
	bIsRetreating = false;

	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = OriginalSpeed;
		}
	}

	if (StateComponent && StateComponent->CachedAIController.IsValid())
	{
		StateComponent->CachedAIController->StopMovement();
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
