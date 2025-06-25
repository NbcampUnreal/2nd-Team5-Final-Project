#include "BoidMovementComponent.h"

#include "AIController.h"
#include "NavigationSystem.h"
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
		SetComponentTickEnabled(false);
		return;
	}

	SwarmManager = OwnerCharacter->GetMySwarmManager();
	if (!SwarmManager)
	{
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

	if (Monster->HasStrategyState(TAG_AI_IsPlayingMontage) || Monster->HasBattleState(TAG_AI_Dead)) return;

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
	if (!Agent) return;
	
	const ASwarmAgent* Leader = SwarmManager->GetLeader();
	if (!Leader) return;

	AAIController* LeaderController = Cast<AAIController>(Leader->GetController());
	const UBlackboardComponent* BlackboardComp =
		LeaderController ? LeaderController->GetBlackboardComponent() : nullptr;
	if (!BlackboardComp) return;

	//const AActor* CurrentTarget = Agent->CurrentDetectedActor;
	const AActor* CurrentTarget = SwarmManager->CurrentSquadTarget;
	if (!CurrentTarget)
	{
		SwarmManager->SetSquadState(ESquadState::Patrolling_Move);
		return;
	}

	if (CurrentState == EBoidMonsterState::FS_Retreating) // 후퇴 분기
	{
		const float DistSqToRetreatTarget = FVector::DistSquared(OwnerCharacter->GetActorLocation(), RetreatTargetLocation);

		if (DistSqToRetreatTarget < FMath::Square(100.0f))
		{
			CurrentState = EBoidMonsterState::FS_UnAbleToAttack;
		}
		else
		{
			const FVector DirectionToRetreatTarget = (RetreatTargetLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();
			OwnerCharacter->AddMovementInput(DirectionToRetreatTarget);

			const FVector DirectionToEnemy = (CurrentTarget->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
			const FRotator TargetRotation = DirectionToEnemy.Rotation();
			OwnerCharacter->SetActorRotation(
				FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 10.0f));
		}
	}
	else // 공격 분기
	{
		TotalTime = GetWorld()->GetTimeSeconds() - Agent->LastAttackFinishTime;
		if (TotalTime > CurrentCoolDown)
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
			AbleToPlayWonderMontage = true;
		}
		else
		{
			/*
			const FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - OwnerCharacter->
					GetActorLocation()).
				GetSafeNormal();
			const FRotator TargetRotation = DirectionToTarget.Rotation();
			OwnerCharacter->SetActorRotation(
				FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, 10.0f));
			*/
			// 공격 분기의 이동 처리
			HandleMovementState(DeltaTime, Agent);
		}
	}
}

void UBoidMovementComponent::HandleMovementState(float DeltaTime, ASwarmAgent* Agent)
{
	UCharacterMovementComponent* MoveComp = Agent->GetCharacterMovement();
	if (!MoveComp) return;
	
	float CurrentSeparation, CurrentCohesion, CurrentAlignment, CurrentGoalSeeking;

	if (SwarmManager->GetCurrentSquadState() == ESquadState::Engaging)
	{
		MoveComp->bOrientRotationToMovement = true;
		
		CurrentSeparation = SwarmManager->CombatSeparation;
		CurrentCohesion = SwarmManager->CombatCohesion;
		CurrentAlignment = SwarmManager->CombatAlignment;
		CurrentGoalSeeking = SwarmManager->CombatGoalSeeking;
	}
	else
	{
		MoveComp->bOrientRotationToMovement = true;
		
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
			CurrentSeparation = FMath::RandRange(50000, 70000);
		}
	}

	const FVector SeparationForce = CalculateSeparationForce(Neighbors) * CurrentSeparation;
	const FVector AlignmentForce = CalculateAlignmentForce(Neighbors) * CurrentAlignment;
	const FVector CohesionForce = CalculateCohesionForce(Neighbors) * CurrentCohesion;

	const float SeparationSq = SeparationForce.SizeSquared();

	FVector FinalGoalForce = FVector::ZeroVector; 

	if (SwarmManager->GetCurrentSquadState() == ESquadState::Engaging
		&& CurrentState == EBoidMonsterState::FS_UnAbleToAttack)
	{
		if (SeparationSq > 600) // 뒤에있어서 멀어지려는 힘이 약하고
		{
			OwnerCharacter->GetCharacterMovement()->StopMovementImmediately(); // 멈춰 or Anim
			AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(Agent);
			if (!Monster) return;
			if (AbleToPlayWonderMontage)
			{
				Monster->PlayETCAnim();
				AbleToPlayWonderMontage = false;
			}
		}
	}
	else
	{
		const FVector GoalSeekingForce = CalculateGoalSeekingForce();
		FinalGoalForce = GoalSeekingForce * CurrentGoalSeeking;
	}

	FVector SteeringForce = FinalGoalForce + SeparationForce + AlignmentForce + CohesionForce;
	SteeringForce = SteeringForce.GetClampedToMaxSize(MaxSteeringForce);

	const float InterpSpeed = 50.0f;
	SmoothedSteeringForce = FMath::VInterpTo(SmoothedSteeringForce, SteeringForce, DeltaTime, InterpSpeed);

	OwnerCharacter->GetCharacterMovement()->AddInputVector(SmoothedSteeringForce);
}

void UBoidMovementComponent::CheckAndHandleStuckTeleport(float DeltaTime)
{
	const FVector GoalLocation = GetGoalLocation();
	if (GoalLocation.IsZero()) return;

	const FVector MyLocation = OwnerCharacter->GetActorLocation();
	//DrawDebugLine(GetWorld(), MyLocation, GoalLocation, FColor::Yellow, false, 0.1f, 0, 1.0f);

	const float DistanceToGoal = FVector::Dist(OwnerCharacter->GetActorLocation(), GoalLocation);
	if (DistanceToGoal < StuckDistanceThreshold)
	{
		return;
	}

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
		true,
		FColor::Red,
		FColor::Green,
		2.0f 
	);

	if (bHitObstacle)
	{
		TArray<AActor*> OverlappedActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> OverlapObjectTypes;
		OverlapObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));

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

		//FColor SphereColor = bIsOccupied ? FColor::Red : FColor::Green;
		//DrawDebugSphere(GetWorld(), GoalLocation, CharacterCapsuleRadius, 16, SphereColor, false, 2.0f, 0, 2.0f);

		if (!bIsOccupied)
		{
			if (!GetWorld()->GetTimerManager().IsTimerActive(RestoreCollisionTimerHandle))
			{
				if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent())
				{
					CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
				}
			
				if (TeleportEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TeleportEffect, GoalLocation);
				}
				if (TeleportSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), TeleportSound, GoalLocation);
				}

				OwnerCharacter->SetActorLocation(GoalLocation, false, nullptr, ETeleportType::TeleportPhysics);

				GetWorld()->GetTimerManager().SetTimer(
				   RestoreCollisionTimerHandle,
				   this,
				   &UBoidMovementComponent::RestorePawnCollision,
				   PostTeleportCollisionGracePeriod,
				   false
			   );
			}
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

void UBoidMovementComponent::RestorePawnCollision()
{
	if (OwnerCharacter)
	{
		if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent())
		{
			CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		}
	}
}

void UBoidMovementComponent::BeginAttack(const float DeltaTime, const AActor* CurrentTarget, ASwarmAgent* Agent)
{
	if (OwnerCharacter)
	{
		if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
		{
			OriginalMaxWalkSpeed = MoveComp->MaxWalkSpeed;
			MoveComp->MaxWalkSpeed = RetreatSpeed;
		}
	}

	CurrentState = EBoidMonsterState::FS_Retreating;
	const FVector DirectionFromTarget = (OwnerCharacter->GetActorLocation() - CurrentTarget->GetActorLocation()).GetSafeNormal();
	RetreatTargetLocation = OwnerCharacter->GetActorLocation() + DirectionFromTarget * RetreatDistance;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLocation;
	if (NavSys && NavSys->GetRandomPointInNavigableRadius(RetreatTargetLocation, 50.f, NavLocation))
	{
	    RetreatTargetLocation = NavLocation.Location;
	}
	
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
		CurrentCoolDown = FMath::RandRange(MinAttackCoolDown, MaxAttackCoolDown);
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

	if (CurrentSquadState == ESquadState::Engaging)
	{
		const ASwarmAgent* Leader = SwarmManager->GetLeader();
		if (!Leader) return FVector::ZeroVector;

		AAIController* LeaderController = Cast<AAIController>(Leader->GetController());
		const UBlackboardComponent* BlackboardComp = LeaderController
			                                             ? LeaderController->GetBlackboardComponent()
			                                             : nullptr;
		if (!BlackboardComp) return FVector::ZeroVector;

		const AActor* TargetPlayer = SwarmManager->CurrentSquadTarget;
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
	else
	{
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
	}

	return FVector::ZeroVector;
}
