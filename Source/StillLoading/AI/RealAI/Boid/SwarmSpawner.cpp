#include "SwarmSpawner.h"

#include "NiagaraFunctionLibrary.h"
#include "SwarmAgent.h"
#include "SwarmManager.h"
#include "AnimInstances/SLCompanionAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASwarmSpawner::ASwarmSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;

	SpawnBox->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnBox->SetCanEverAffectNavigation(false);
}

void ASwarmSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FGenericTeamId InvalidTeamID = FGenericTeamId::NoTeam;
	const FGenericTeamId DefaultTeamID = FGenericTeamId(2);

	for (FSwarmComposition& Composition : SwarmCompositions)
	{
		if (Composition.TeamIDToAssign == InvalidTeamID)
		{
			Composition.TeamIDToAssign = DefaultTeamID;
		}
	}
}

void ASwarmSpawner::BeginSpawn()
{
	UWorld* World = GetWorld();
	if (World && SwarmManagerClass)
	{
		if (SpawnEffectTemplate)
		{
			const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, EffectSpawnHeightOffset);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				SpawnEffectTemplate,
				SpawnLocation,
				EffectSpawnRotation
			);
		}

		SpawnedManager = World->SpawnActor<ASwarmManager>(SwarmManagerClass, GetActorLocation(), GetActorRotation());
		if (SpawnedManager)
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
			SpawnedManager->DetectionRadius = DetectionRadius;
			SpawnedManager->OnMonstersUpdated.AddDynamic(this, &ASwarmSpawner::OnMonstersUpdated_Handler);

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

			TotalSpawnCount = 0;

			for (const auto& [AgentClass, ControllerClass, SpawnCount, AvoidanceWeight, bIsLeader, TeamIDToAssign] :
			     SwarmCompositions)
			{
				if (AgentClass)
				{
					TotalSpawnCount += SpawnCount;

					for (int32 i = 0; i < SpawnCount; ++i)
					{
						const FBox SpawnableArea = FBox(GetActorLocation() - SpawnBox->GetScaledBoxExtent(),
						                                GetActorLocation() + SpawnBox->GetScaledBoxExtent());
						const FVector SpawnLocation = FMath::RandPointInBox(SpawnableArea);
						const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

						constexpr ESpawnActorCollisionHandlingMethod CollisionHandlingOverride =
							ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

						// 지연 스폰
						if (ASwarmAgent* DeferredAgent = World->SpawnActorDeferred<ASwarmAgent>(
							AgentClass, SpawnTransform, this, nullptr, CollisionHandlingOverride))
						{
							DeferredAgent->AIControllerClass = ControllerClass;
							DeferredAgent->MySwarmManager = SpawnedManager;

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
								if (bBeginBurserkMode)
								{
									DeferredAgent->RequestBerserkMode();
								}

								DeferredAgent->SetLeader(true, LeaderBehaviorTree, LeaderBlackBoard);
								SpawnedManager->SetLeader(DeferredAgent);
							}

							UGameplayStatics::FinishSpawningActor(DeferredAgent, SpawnTransform);

							if (AAIController* AIController = Cast<AAIController>(DeferredAgent->GetController()))
							{
								AIController->SetGenericTeamId(TeamIDToAssign);
							}

							SpawnedManager->RegisterAgent(DeferredAgent); // 에이전트 등록
						}
						else
						{
							UE_LOG(LogTemp, Error,
							       TEXT("ASwarmSpawner::BeginSpawn - Failed to spawn agent of class %s at location %s"),
							       *AgentClass->GetName(),
							       *SpawnLocation.ToString());
						}
					}
				}
			}
		}
	}
}

int32 ASwarmSpawner::GetSpawnCount() const
{
	return TotalSpawnCount;
}

// TODO::무기 없애기
void ASwarmSpawner::ResetSpawendMonster()
{
	DestroyAllMonster();
	BeginSpawn();
}

void ASwarmSpawner::DestroyAllMonster()
{
	if (IsValid(SpawnedManager))
	{
		SpawnedManager->DestroyAllAgents();
		SpawnedManager->Destroy();
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

void ASwarmSpawner::OnMonstersUpdated_Handler(const int32 DecreaseCount)
{
	OnMonstersUpdated.Broadcast(DecreaseCount);
	UE_LOG(LogTemp, Warning, TEXT("DecreaseCount MonsterCount[%d]"), DecreaseCount);
}
