#include "BoidMovementComponent.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmAgent.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UBoidMovementComponent::UBoidMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBoidMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASwarmAgent>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BoidMovementComponent's Owner is not a valid ASwarmAgent!"));
		SetComponentTickEnabled(false);
		return;
	}

	SwarmManager = OwnerCharacter->GetMySwarmManager();
	if (!SwarmManager)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("BoidMovementComponent could not find its SwarmManager from its Owner Agent! Disabling Tick."),
		       *OwnerCharacter->GetName());
		SetComponentTickEnabled(false);
	}

	OrbitDirection = FMath::RandBool() ? 1.0f : -1.0f;
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

	CheckAndHandleStuckTeleport(DeltaTime);

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

	// 공격 분기
	TotalTime = GetWorld()->GetTimeSeconds() - Agent->LastAttackFinishTime;
	/*
	UE_LOG(LogTemp, Warning, TEXT("Total Time[%f][%s]"), TotalTime, *Agent->GetName());
	UE_LOG(LogTemp, Warning, TEXT("CurrentCollDown[%f][%s]"), CurrentCollDown, *Agent->GetName());
	UE_LOG(LogTemp, Warning, TEXT("LastAttackFinishTime[%f][%s]"), Agent->LastAttackFinishTime, *Agent->GetName());
	*/
	if (TotalTime > CurrentCollDown)
	{
		CurrentState = EBoidMonsterState::FS_AbleToAttack;
	}
	else
	{
		CurrentState = EBoidMonsterState::FS_UnAbleToAttack;
	}

	const float DistanceToTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistanceToTarget < AttackRange && CurrentState == EBoidMonsterState::FS_AbleToAttack)
	{
		BeginAttack(DeltaTime, CurrentTarget, Agent);
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

	if (SwarmManager->GetCurrentSquadState() == ESquadState::Engaging)
	{
		if (CurrentState == EBoidMonsterState::FS_UnAbleToAttack)
		{
			CurrentSeparation = ForceDistanceToTarget;
		}
	}

	const FVector SeparationForce = CalculateSeparationForce(Neighbors) * CurrentSeparation;
	const FVector AlignmentForce = CalculateAlignmentForce(Neighbors) * CurrentAlignment;
	const FVector CohesionForce = CalculateCohesionForce(Neighbors) * CurrentCohesion;

	const float SeparationSq = SeparationForce.SizeSquared();
	const float AlignmentSq = AlignmentForce.SizeSquared();
	const float CohesionSq = CohesionForce.SizeSquared();

	FVector FinalGoalForce = FVector::ZeroVector; 

	if (SwarmManager->GetCurrentSquadState() == ESquadState::Engaging
		&& CurrentState == EBoidMonsterState::FS_UnAbleToAttack)
	{
		if (SeparationSq < 1000) // 뒤에있어서 멀어지려는 힘이 약하고
		{
			//OwnerCharacter->GetCharacterMovement()->StopMovementImmediately(); // 멈춰
		}
		//UE_LOG(LogTemp, Warning, TEXT("SeparationSq [%f]"), SeparationSq);

		const FVector PlayerLocation = GetGoalLocation();
		if (!PlayerLocation.IsZero())
		{
			const FVector MyLocation = OwnerCharacter->GetActorLocation();
			const FVector VectorFromPlayerToMe = MyLocation - PlayerLocation;

			FVector OrbitDirectionVector = FVector::CrossProduct(VectorFromPlayerToMe, FVector::UpVector);
			OrbitDirectionVector.Normalize();

			FinalGoalForce = OrbitDirectionVector * OrbitForceWeight * OrbitDirection;
		}
	}
	else
	{
		const FVector GoalSeekingForce = CalculateGoalSeekingForce();
		FinalGoalForce = GoalSeekingForce * CurrentGoalSeeking;
	}

	FVector SteeringForce = FinalGoalForce + SeparationForce + AlignmentForce + CohesionForce;
	SteeringForce = SteeringForce.GetClampedToMaxSize(MaxSteeringForce);

	const float InterpSpeed = 5.0f;
	SmoothedSteeringForce = FMath::VInterpTo(SmoothedSteeringForce, SteeringForce, DeltaTime, InterpSpeed);

	OwnerCharacter->GetCharacterMovement()->AddInputVector(SmoothedSteeringForce);
}

void UBoidMovementComponent::CheckAndHandleStuckTeleport(float DeltaTime)
{
	const FVector GoalLocation = GetGoalLocation();
	if (GoalLocation.IsZero()) return;

	const float DistanceToGoal = FVector::Dist(OwnerCharacter->GetActorLocation(), GoalLocation);
	if (DistanceToGoal < StuckDistanceThreshold)
	{
		return;
	}

	const FVector MyLocation = OwnerCharacter->GetActorLocation();
	FVector MoveDirection = OwnerCharacter->GetVelocity().GetSafeNormal();
	if (MoveDirection.IsNearlyZero())
	{
		MoveDirection = (GoalLocation - MyLocation).GetSafeNormal();
	}

	FHitResult HitResult;
	float CapsuleRadius, CapsuleHalfHeight;
	OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	const bool bHitObstacle = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		MyLocation,
		MyLocation + (MoveDirection * ObstacleCheckDistance),
		CapsuleRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{OwnerCharacter},
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHitObstacle)
	{
		TArray<AActor*> OverlappedActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> OverlapObjectTypes;
		OverlapObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); // Pawn 타입만 검사

		float CharacterCapsuleRadius, CharacterCapsuleHalfHeight;
		OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleSize(CharacterCapsuleRadius, CharacterCapsuleHalfHeight);

		const bool bIsOccupied = UKismetSystemLibrary::SphereOverlapActors(
			GetWorld(),
			GoalLocation,
			CharacterCapsuleRadius,
			OverlapObjectTypes,
			nullptr,
			{OwnerCharacter},
			OverlappedActors
		);

		if (!bIsOccupied)
		{
			if (TeleportEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TeleportEffect, GoalLocation);
			}
			if (TeleportSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), TeleportSound, GoalLocation);
			}

			OwnerCharacter->SetActorLocation(GoalLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Agent %s teleport cancelled, destination is occupied."),
			       *OwnerCharacter->GetName());
		}
	}
}

FVector UBoidMovementComponent::GetGoalLocation() const
{
	const ESquadState CurrentSquadState = SwarmManager->GetCurrentSquadState();
	if (CurrentSquadState == ESquadState::Engaging)
	{
		if (const ASwarmAgent* Leader = SwarmManager->GetLeader())
		{
			if (AAIController* LeaderController = Cast<AAIController>(Leader->GetController()))
			{
				if (const UBlackboardComponent* BlackboardComp = LeaderController->GetBlackboardComponent())
				{
					if (const AActor* TargetPlayer = Cast<
						AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor"))))
					{
						return TargetPlayer->GetActorLocation();
					}
				}
			}
		}
	}
	else
	{
		if (ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(OwnerCharacter))
		{
			if (!OwningAgent->IsLeader())
			{
				return SwarmManager->GetFormationSlotLocationForAgent(OwningAgent->GetAgentID());
			}
		}
	}
	return FVector::ZeroVector;
}

void UBoidMovementComponent::InitializeComponent(const TObjectPtr<ASwarmManager>& InSwarmManager)
{
	this->SwarmManager = InSwarmManager;
}

void UBoidMovementComponent::BeginAttack(const float DeltaTime, const AActor* CurrentTarget, ASwarmAgent* Agent)
{
	AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(Agent);
	if (!Monster) return;

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
		CurrentCollDown = FMath::RandRange(MinAttackCoolDown, MaxAttackCoolDown);
	}
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

	const ESquadState CurrentSquadState = SwarmManager->GetCurrentSquadState();

	// 1. 교전(Engaging) 상태일 때의 목표 설정: 타겟
	/*
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
	*/

	if (CurrentSquadState == ESquadState::Engaging)
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
		const FVector PlayerLocation = TargetPlayer->GetActorLocation();

		const FVector VectorFromPlayerToMe = MyLocation - PlayerLocation;
		const FVector DirectionFromPlayerToMe = VectorFromPlayerToMe.GetSafeNormal();

		const FVector GoalLocation = PlayerLocation + (DirectionFromPlayerToMe * DesiredAttackDistance);
		const FVector VectorToGoal = GoalLocation - MyLocation;

		if (VectorToGoal.Size() < 10.0f)
		{
			return FVector::ZeroVector;
		}

		const float MaxSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		const FVector DesiredVelocity = VectorToGoal.GetSafeNormal() * MaxSpeed;
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
