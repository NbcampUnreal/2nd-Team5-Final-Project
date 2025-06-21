#include "SwarmSpawner.h"

#include "SwarmAgent.h"
#include "SwarmManager.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AnimInstances/SLCompanionAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/SphereComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASwarmSpawner::ASwarmSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnRadiusSphere"));
	RootComponent = SpawnRadiusSphere;

	SpawnRadiusSphere->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnRadiusSphere->SetCanEverAffectNavigation(false);
}

void ASwarmSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SpawnRadiusSphere)
	{
		SpawnRadiusSphere->SetSphereRadius(SpawnRadius);
	}
}

void ASwarmSpawner::BeginSpawn()
{
	UWorld* World = GetWorld();
	if (World && SwarmManagerClass)
	{
		if (ASwarmManager* SpawnedManager = World->SpawnActor<ASwarmManager>(
			SwarmManagerClass, GetActorLocation(), GetActorRotation()))
		{
			// 군체 알고리즘 가중치 셋팅
			SpawnedManager->SwarmPatrolPoints = PatrolPoints;
			SpawnedManager->PatrolSeparation = SeparationWeight;
			SpawnedManager->PatrolAlignment = AlignmentWeight;
			SpawnedManager->PatrolCohesion = CohesionWeight;
			SpawnedManager->PatrolGoalSeeking = GoalSeekingWeight;

			SpawnedManager->CombatSeparation = CombatSeparationWeight;
			SpawnedManager->CombatAlignment = CombatAlignmentWeight;
			SpawnedManager->CombatCohesion = CombatCohesionWeight;
			SpawnedManager->CombatGoalSeeking = CombatGoalSeekingWeight;

			SpawnedManager->CurrentFormationType = FormationType;

			// 순찰 포인트 정렬
			TArray<FVector> InitialPathPoints;
			for (const ATargetPoint* Point : PatrolPoints)
			{
				if (Point)
				{
					InitialPathPoints.Add(Point->GetActorLocation());
				}
			}

			if (InitialPathPoints.Num() > 0)
			{
				SpawnedManager->SetNewPath(InitialPathPoints);
			}

			for (const auto& [AgentClass, ControllerClass, SpawnCount, AvoidanceWeight, bIsLeader] : SwarmCompositions)
			{
				if (AgentClass)
				{
					for (int32 i = 0; i < SpawnCount; ++i)
					{
						const FVector SpawnLocation = GetActorLocation() + FMath::VRand() * FMath::FRandRange(0, SpawnRadius);
						const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

						// 지연 스폰
						if (ASwarmAgent* DeferredAgent = World->SpawnActorDeferred<ASwarmAgent>(AgentClass, SpawnTransform))
						{
							DeferredAgent->AIControllerClass = ControllerClass;

							if (UCharacterMovementComponent* MoveComp = DeferredAgent->GetCharacterMovement())
							{
								MoveComp->AvoidanceWeight = AvoidanceWeight; // AVO 셋팅

								if (bIsLeader)
								{
									MoveComp->MaxWalkSpeed = LeaderMovementSpeed;
								}
								else
								{
									MoveComp->MaxWalkSpeed = FollowerMovementSpeed;
								}
							}

							if (bIsLeader)
							{
								DeferredAgent->RequestBerserkMode();
								DeferredAgent->SetLeader(true, LeaderBehaviorTree, LeaderBlackBoard);
								SpawnedManager->SetLeader(DeferredAgent);
							}

							UGameplayStatics::FinishSpawningActor(DeferredAgent, SpawnTransform);
							SpawnedManager->RegisterAgent(DeferredAgent); // 에이전트 등록
						}
					}
				}
			}
		}
	}
}

void ASwarmSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bEnableAutoSpawn)
	{
		BeginSpawn();
	}
}
