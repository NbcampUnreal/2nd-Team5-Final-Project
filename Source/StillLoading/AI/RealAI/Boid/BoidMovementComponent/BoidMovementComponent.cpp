#include "BoidMovementComponent.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmAgent.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBoidMovementComponent::UBoidMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBoidMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	SwarmManager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
}

void UBoidMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(GetOwner());
	if (OwningAgent && OwningAgent->IsLeader())
	{
		if (IsComponentTickEnabled())
		{
			SetComponentTickEnabled(false);
			UE_LOG(LogTemp, Warning, TEXT("BoidMovementComponent DEACTIVATED for Leader: %s"), *OwningAgent->GetName());
		}
		return;
	}

	if (!OwnerCharacter || !SwarmManager) return;
	AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(OwningAgent);
	if (!Monster) return;
	if (Monster->HasStrategyState(TAG_AI_IsPlayingMontage)) return;

	const ESquadState SquadState = SwarmManager->GetCurrentSquadState();

	switch (SquadState)
	{
	case ESquadState::Patrolling_Move:
		// '이동'
		HandleMovementState(DeltaTime, Monster);
		break;

	case ESquadState::Patrolling_Wait:
		// '대기'
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		if (UAnimationMontageComponent* AnimComp = Monster->FindComponentByClass<UAnimationMontageComponent>())
		{
			AnimComp->PlayAIETCMontage("WaitA");
			Monster->SetStrategyState(TAG_AI_IsPlayingMontage);
		}
		break;

	case ESquadState::Engaging:
		// '교전'
		Monster->SetPrimaryState(TAG_AI_AbleToAttack);
		HandleCombatState(DeltaTime, Monster);
		break;
	}
}

void UBoidMovementComponent::HandleCombatState(float DeltaTime, ASwarmAgent* Agent)
{
	const ASwarmAgent* Leader = SwarmManager->GetLeader();
	if (!Leader) return;

	AAIController* LeaderController = Cast<AAIController>(Leader->GetController());
	const UBlackboardComponent* BlackboardComp =
		LeaderController ? LeaderController->GetBlackboardComponent() : nullptr;
	if (!BlackboardComp) return;

	const AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	if (!Agent || !CurrentTarget) return;

	const float DistanceToTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistanceToTarget < AttackRange)
	{
		AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(Agent);
		if (!Monster) return;

		// 공격 분기
		if (GetWorld()->GetTimeSeconds() - Agent->LastAttackFinishTime > AttackCoolDown)
		{
			if (!Monster->IsInPrimaryState(TAG_AI_IsAttacking) &&
				!Monster->HasStrategyState(TAG_AI_IsPlayingMontage))
			{
				OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();

				const FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - OwnerCharacter->
						GetActorLocation()).
					GetSafeNormal();
				const FRotator TargetRotation = DirectionToTarget.Rotation();
				OwnerCharacter->SetActorRotation(
					FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 10.0f));

				Monster->PlayAttackAnim();
			}
		}
	}
	else
	{
		const FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - OwnerCharacter->
				GetActorLocation()).
			GetSafeNormal();
		const FRotator TargetRotation = DirectionToTarget.Rotation();
		OwnerCharacter->SetActorRotation(
			FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 10.0f));

		// 공격 분기의 이동 처리
		HandleMovementState(DeltaTime, Agent);
	}
}

void UBoidMovementComponent::HandleMovementState(float DeltaTime, ASwarmAgent* Agent)
{
	float CurrentSeparation, CurrentCohesion, CurrentAlignment, CurrentGoalSeeking;

	if (SwarmManager->GetCurrentSquadState() == ESquadState::Engaging)
	{
		CurrentSeparation = SwarmManager->CombatSeparation;
		CurrentCohesion = SwarmManager->CombatCohesion;
		CurrentAlignment = SwarmManager->CombatAlignment;
		CurrentGoalSeeking = SwarmManager->CombatGoalSeeking;
	}
	else
	{
		CurrentSeparation = SwarmManager->PatrolSeparation;
		CurrentCohesion = SwarmManager->PatrolCohesion;
		CurrentAlignment = SwarmManager->PatrolAlignment;
		CurrentGoalSeeking = SwarmManager->PatrolGoalSeeking;
	}

	TArray<AActor*> Neighbors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), OwnerCharacter->GetActorLocation(), this->PerceptionRadius, ObjectTypes,
		AActor::StaticClass(), TArray<AActor*>({OwnerCharacter}), Neighbors);

	const FVector SeparationForce = CalculateSeparationForce(Neighbors) * CurrentSeparation;
	const FVector AlignmentForce = CalculateAlignmentForce(Neighbors) * CurrentAlignment;
	const FVector CohesionForce = CalculateCohesionForce(Neighbors) * CurrentCohesion;
	const FVector GoalSeekingForce = CalculateGoalSeekingForce();

	FVector SteeringForce = (GoalSeekingForce * CurrentGoalSeeking) + SeparationForce + AlignmentForce + CohesionForce;
	SteeringForce = SteeringForce.GetClampedToMaxSize(MaxSteeringForce);

	const float InterpSpeed = 0.5f;
	SmoothedSteeringForce = FMath::VInterpTo(SmoothedSteeringForce, SteeringForce, DeltaTime, InterpSpeed);
	
	OwnerCharacter->GetCharacterMovement()->AddInputVector(SmoothedSteeringForce);
}

FVector UBoidMovementComponent::CalculateSeparationForce(const TArray<AActor*>& Neighbors)
{
	FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;

	for (const AActor* Neighbor : Neighbors)
	{
		FVector ToNeighbor = OwnerCharacter->GetActorLocation() - Neighbor->GetActorLocation();
		if (ToNeighbor.Size() > 0)
		{
			// 방향은 유지하되(GetSafeNormal), 거리에 반비례 해서 힘의 크기를 조절
			Force += ToNeighbor.GetSafeNormal() / ToNeighbor.Size();
		}
	}
	return Force / Neighbors.Num();
}

FVector UBoidMovementComponent::CalculateAlignmentForce(const TArray<AActor*>& Neighbors)
{
	FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;

	for (const AActor* Neighbor : Neighbors)
	{
		Force += Neighbor->GetVelocity();
	}

	// 방향 정보만 사용
	return (Force / Neighbors.Num()).GetSafeNormal();
}

FVector UBoidMovementComponent::CalculateCohesionForce(const TArray<AActor*>& Neighbors)
{
	const FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;

	FVector CenterOfMass = FVector::ZeroVector;
	for (const AActor* Neighbor : Neighbors)
	{
		// 모든 주변 동료들의 현재 위치를 더함
		CenterOfMass += Neighbor->GetActorLocation();
	}

	// 더해진 위치 값의 평균을 구하여, 무리의 무게 중심 계산
	CenterOfMass /= Neighbors.Num();

	return (CenterOfMass - OwnerCharacter->GetActorLocation()).GetSafeNormal();
}

FVector UBoidMovementComponent::CalculateGoalSeekingForce()
{
	if (!SwarmManager || !OwnerCharacter) return FVector::ZeroVector;

	const ESquadState CurrentState = SwarmManager->GetCurrentSquadState();

	// 1. 교전(Engaging) 상태일 때의 목표 설정: 타겟
	if (CurrentState == ESquadState::Engaging)
	{
		const ASwarmAgent* Leader = SwarmManager->GetLeader();
		if (!Leader) return FVector::ZeroVector;

		AAIController* LeaderController = Cast<AAIController>(Leader->GetController());
		const UBlackboardComponent* BlackboardComp = LeaderController
			                                             ? LeaderController->GetBlackboardComponent()
			                                             : nullptr;
		if (!BlackboardComp) return FVector::ZeroVector;

		const AActor* TargetPlayer = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
		if (!TargetPlayer) return FVector::ZeroVector;

		const FVector MyLocation = OwnerCharacter->GetActorLocation();
		const FVector VectorToTarget = TargetPlayer->GetActorLocation() - MyLocation;
		const float DistanceToTarget = VectorToTarget.Size();

		const float DesiredStoppingDistance = 100.0f;
		if (DistanceToTarget <= DesiredStoppingDistance)
		{
			return FVector::ZeroVector;
		}

		const float MaxSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		const FVector DesiredVelocity = VectorToTarget.GetSafeNormal() * MaxSpeed;
		return DesiredVelocity - OwnerCharacter->GetVelocity();
	}

	const ASwarmAgent* Leader = SwarmManager->GetLeader();
	ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(OwnerCharacter);

	if (Leader && OwningAgent && !OwningAgent->IsLeader())
	{
		const FVector MyFormationSlot = SwarmManager->GetFormationSlotLocationForAgent(OwningAgent->GetAgentID());
		if (MyFormationSlot.IsZero()) return FVector::ZeroVector;

		const FVector ToSlot = MyFormationSlot - OwnerCharacter->GetActorLocation();
		const float DistanceToSlot = ToSlot.Size();

		const float MaxSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		FVector DesiredVelocity = ToSlot.GetSafeNormal() * MaxSpeed;

		if (DistanceToSlot < ArrivalSlowingRadius)
		{
			DesiredVelocity *= (DistanceToSlot / ArrivalSlowingRadius);
		}

		return DesiredVelocity - OwnerCharacter->GetVelocity();
	}

	return FVector::ZeroVector;
}
