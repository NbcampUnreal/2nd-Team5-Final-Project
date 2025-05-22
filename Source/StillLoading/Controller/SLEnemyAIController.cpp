// Fill out your copyright notice in the Description page of Project Settings.


#include "SLEnemyAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

class SLMonster;

const FName ASLEnemyAIController::PatrolLocationKey = "PatrolLocation";

ASLEnemyAIController::ASLEnemyAIController()
{
	AAIController::SetGenericTeamId(FGenericTeamId(2));
	AISenseConfig_Sight->SightRadius = DetectionRadius;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 80.f;
	AISenseConfig_Sight->LoseSightRadius = LoseInterestRadius;
}

void ASLEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	BlackboardComp = GetBlackboardComponent();
	BlackboardComp->SetValueAsBool("IsPatrolState", IsPatrolState);
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
				IsPatrolState = false;
				BlackboardComponent->SetValueAsBool("IsPatrolState", IsPatrolState);
				TargetActor = Actor;
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
		else
		{
			GetBlackboardComponent()->ClearValue(FName("TargetActor"));
		}
	}
}

void ASLEnemyAIController::InitializePatrolPoints()
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

void ASLEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess (InPawn);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASLEnemyAIController::InitializePatrolPoints, 0.5f, false);
}

void ASLEnemyAIController::SetPeripheralVisionAngle(float NewAngle)
{
	if (AISenseConfig_Sight)
	{
		AISenseConfig_Sight->PeripheralVisionAngleDegrees = NewAngle;
		GetPerceptionComponent()->RequestStimuliListenerUpdate();
	}
}
