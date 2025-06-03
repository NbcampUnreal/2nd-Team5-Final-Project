// Fill out your copyright notice in the Description page of Project Settings.


#include "SLNPCAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

const FName ASLNPCAIController::PatrolLocationKey = "PatrolLocation";

ASLNPCAIController::ASLNPCAIController()
{
	AAIController::SetGenericTeamId(FGenericTeamId(1));
	AISenseConfig_Sight->SightRadius = DetectionRadius;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 50.f;
	AISenseConfig_Sight->LoseSightRadius = LoseInterestRadius;
}

void ASLNPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateAIState();

	if (CurrentState == EAIState::EAIS_Waiting)
	{
		WaitTime -= DeltaTime;
		if (WaitTime <= 0.f)
		{
			CurrentState = EAIState::EAIS_Combat;
		}
	}
}

ETeamAttitude::Type ASLNPCAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
	FGenericTeamId MyTeamID = GetGenericTeamId();
	
	// 팀 ID가 같으면 우호적
	if (OtherTeamID == MyTeamID)
	{
		return ETeamAttitude::Friendly;
	}
	else
	{
		return ETeamAttitude::Hostile;
	}
}

void ASLNPCAIController::BeginPlay()
{
	Super::BeginPlay();

	BlackboardComp = GetBlackboardComponent();
	BlackboardComp->SetValueAsBool("IsPatrolState", IsPatrolState);
}

/*void ASLNPCAIController::UpdateAIState()
{*/
	/*if (!TargetActor) return;
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
	float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());

	switch (CurrentState)
	{
	case EAIState::EAIS_Idle:
		if (DistanceToTarget <= DetectionRadius)
		{
			CurrentState = EAIState::EAIS_Suspicious;
		}
		break;
	case EAIState::EAIS_Suspicious:
		if (DistanceToTarget <= ChaseRadius)
		{
			StartChasing(TargetActor);
		}
		else if (DistanceToTarget > DetectionRadius)
		{
			CurrentState = EAIState::EAIS_Idle;
		}
		break;
	case EAIState::EAIS_Chasing:

		IsPatrolState = false;
		BlackboardComp->SetValueAsBool("IsPatrolState", IsPatrolState);

		MoveToActor(TargetActor, 250.f);

		if (DistanceToTarget <= CombatRadius)
		{
			bHasFixedTarget = true;
			/*StopMovement();
			FVector RightVector = ControlledPawn->GetActorRightVector();
			FVector FrontVector = ControlledPawn->GetActorForwardVector();
			FVector CurrentLocation = ControlledPawn->GetActorLocation();

			float StepDistance = 600.f;
			float ForwardDistance = 400.f;
			FVector NewLocation = CurrentLocation + RightVector * StepDistance + FrontVector* ForwardDistance;
			FAIMoveRequest MoveRequest;

			ASLNPC* NPC = Cast<ASLNPC>(GetPawn());
			if (NPC)
			{
				UCharacterMovementComponent* MovementComp = NPC->GetCharacterMovement();
				if (MovementComp)
				{
					MovementComp->MaxWalkSpeed = 110.f;
					MovementComp->bOrientRotationToMovement = false;
				}
			}

			MoveToLocation(NewLocation, 10.f);

			CurrentState = EAIState::Waiting;#1#

			CurrentState = EAIState::EAIS_Combat;
			
		}
		else if (DistanceToTarget > LoseInterestRadius)
		{
			StopChasing();
		}
		break;
	case EAIState::EAIS_Waiting:

		
		break;

	case EAIState::EAIS_Combat:

		ASLNPC* NPC = Cast<ASLNPC>(GetPawn());
		if (NPC)
		{
			UCharacterMovementComponent* MovementComp = NPC->GetCharacterMovement();
			if (MovementComp)
			{
				MovementComp->MaxWalkSpeed = 600.f;
				MovementComp->bOrientRotationToMovement = true;
			}
		}

		bIsChasing = false;
		AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f;

		USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(NPC->GetMesh()->GetAnimInstance());

		FVector CurrentLocation = ControlledPawn->GetActorLocation();
		FVector ToTarget = TargetActor->GetActorLocation() - CurrentLocation;
		ToTarget.Normalize();
		float Dot = FVector::DotProduct(ControlledPawn -> GetActorForwardVector(), ToTarget);


		if (DistanceToTarget <= AttackRange )
		{
			if (Dot >= 0.85f)
			{
				StopMovement();
				NPC->Attack();
			}
			else
			{
				NPC->bUseControllerRotationYaw = true;
				SetFocus(TargetActor);
				if (DistanceToTarget < CombatRadius && AnimInstance->GetIsAttacking() == false)
				{
					FVector Forward = ControlledPawn->GetActorForwardVector();
					FVector TargetLocation = CurrentLocation - Forward * 300.f;
					MoveToLocation(TargetLocation);
				} 
			}
		}
		else if (DistanceToTarget < CombatRadius && AnimInstance->GetIsAttacking() == true)
		{
			/*NPC->bUseControllerRotationYaw = false;
			ClearFocus(EAIFocusPriority::Gameplay);#1#
			StopMovement();
		}
		else
		{
				MoveToActor(TargetActor, 110.f);
				//AnimInstance->SetIsAttacking(false);
			
		}
		break;
	}
}*/

/*void ASLNPCAIController::StopChasing()
{
	bIsChasing = false;
	StopMovement();
	CurrentState = EAIState::EAIS_Idle;
}

void ASLNPCAIController::StartChasing(AActor* Target)
{
	//TargetActor = Target;
	bIsChasing = true;

	ASLNPC* NPC = Cast<ASLNPC>(GetPawn());
	if (NPC)
	{
		UCharacterMovementComponent* MovementComp = NPC->GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed = 650.f;
		}
	}

	if (Target)
	{
		LastKnownLocation = Target->GetActorLocation();
	}
	CurrentState = EAIState::EAIS_Chasing;
}

void ASLNPCAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (bHasFixedTarget) return;
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (Stimulus.WasSuccessfullySensed() && Actor)
		{
			if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
			{
				//TargetActor = Actor;
				APawn* ControlledPawn = GetPawn();
				if (ControlledPawn)
				{
					float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Actor->GetActorLocation());

					if (Distance <= ChaseRadius)
					{
						StartChasing(Actor);
					}
					else if (Distance <= DetectionRadius)
					{
						CurrentState = EAIState::EAIS_Suspicious;
					}
				}
				
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
		else
		{
			
			GetBlackboardComponent()->ClearValue(FName("TargetActor"));
			if (CurrentState == EAIState::EAIS_Chasing)
			{
				MoveToLocation(LastKnownLocation, 100.f);
				CurrentState = EAIState::EAIS_Suspicious;
			}
		}
	}
}

void ASLNPCAIController::InitializePatrolPoints()
{
	PatrolPoints.Empty();
	if (!GetPawn()) return;

	FVector Origin = GetPawn()->GetActorLocation();
	float Radius = 2500.f;
	int32 NumPoints = 7;

	for (int32 i = 0; i < NumPoints; ++i)
	{
		FVector RandomDirection = FMath::VRand();
		RandomDirection.Z = 0.f;

		FVector RandomOffset = RandomDirection * FMath::FRandRange(200.f, Radius);
		FVector TestPoint = Origin + RandomOffset;

		FHitResult HitResult;
		FVector TraceStart = TestPoint + FVector(0.f, 0.f, 1000.f);
		FVector TraceEnd = TestPoint - FVector(0.f, 0.f, 1000.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetPawn());

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_WorldStatic,
			QueryParams
		);

		if (bHit && HitResult.bBlockingHit)
		{
			FVector GroundPoint = HitResult.ImpactPoint;
			PatrolPoints.Add(GroundPoint);
		}
	}
	if (PatrolPoints.Num() > 0)
	{
		Blackboard->SetValueAsVector("PatrolLocation", PatrolPoints[0]);
		Blackboard->SetValueAsInt("PatrolLocation", 0);
	}
	bPatrolPointsReady = true;
}

void ASLNPCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASLNPCAIController::InitializePatrolPoints, 0.5f, false);
}*/

