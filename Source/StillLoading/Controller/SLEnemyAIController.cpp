// Fill out your copyright notice in the Description page of Project Settings.


#include "SLEnemyAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"

ASLEnemyAIController::ASLEnemyAIController()
{
	AAIController::SetGenericTeamId(FGenericTeamId(2));
	AISenseConfig_Sight->SightRadius = DetectionRadius;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 50.f;
	AISenseConfig_Sight->LoseSightRadius = LoseInterestRadius;
}

void ASLEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAIState();

	if (CurrentState == EAIState::Waiting)
	{
		WaitTime -= DeltaTime;
		if (WaitTime <= 0.f)
		{
			CurrentState = EAIState::Combat;
		}
	}
}

ETeamAttitude::Type ASLEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
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

void ASLEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASLEnemyAIController::UpdateAIState()
{
	if (!TargetActor) return;
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
	float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());

	switch (CurrentState)
	{
	case EAIState::Idle:
		if (DistanceToTarget <= DetectionRadius)
		{
			CurrentState = EAIState::Suspicious;
		}
		break;
	case EAIState::Suspicious:
		if (DistanceToTarget <= ChaseRadius)
		{
			StartChasing(TargetActor);
		}
		else if (DistanceToTarget > DetectionRadius)
		{
			CurrentState = EAIState::Idle;
		}
		break;
	case EAIState::Chasing:

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

			CurrentState = EAIState::Waiting;*/

			CurrentState = EAIState::Combat;

		}
		else if (DistanceToTarget > LoseInterestRadius)
		{
			StopChasing();
		}
		break;
	case EAIState::Waiting:


		break;

	case EAIState::Combat:

		ASLMonster* Monster = Cast<ASLMonster>(GetPawn());
		if (Monster)
		{
			UCharacterMovementComponent* MovementComp = Monster->GetCharacterMovement();
			if (MovementComp)
			{
				MovementComp->MaxWalkSpeed = 600.f;
				MovementComp->bOrientRotationToMovement = true;
			}
		}

		bIsChasing = false;
		AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f;

		USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(Monster->GetMesh()->GetAnimInstance());

		FVector CurrentLocation = ControlledPawn->GetActorLocation();
		FVector ToTarget = TargetActor->GetActorLocation() - CurrentLocation;
		ToTarget.Normalize();
		float Dot = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), ToTarget);


		if (DistanceToTarget <= AttackRange)
		{
			if (Dot >= 0.85f)
			{
				StopMovement();
				Monster->Attack();
			}
			else
			{
				Monster->bUseControllerRotationYaw = true;
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
			ClearFocus(EAIFocusPriority::Gameplay);*/
			StopMovement();
		}
		else
		{
			MoveToActor(TargetActor, 110.f);
			AnimInstance->SetIsAttacking(false);

		}


		break;
	}
}

void ASLEnemyAIController::StopChasing()
{
	bIsChasing = false;
	StopMovement();
	CurrentState = EAIState::Idle;
}

void ASLEnemyAIController::StartChasing(AActor* Target)
{
	TargetActor = Target;
	bIsChasing = true;

	if (Target)
	{
		LastKnownLocation = Target->GetActorLocation();
	}
	CurrentState = EAIState::Chasing;

}

void ASLEnemyAIController::OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (bHasFixedTarget) return;
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (Stimulus.WasSuccessfullySensed() && Actor)
		{
			if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
			{
				TargetActor = Actor;
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
						CurrentState = EAIState::Suspicious;
					}
				}

				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
		else
		{

			GetBlackboardComponent()->ClearValue(FName("TargetActor"));
			if (CurrentState == EAIState::Chasing)
			{
				MoveToLocation(LastKnownLocation, 100.f);
				CurrentState = EAIState::Suspicious;
			}
		}
	}
}

