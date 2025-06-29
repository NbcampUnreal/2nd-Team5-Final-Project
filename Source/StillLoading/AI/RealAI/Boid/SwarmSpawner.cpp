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

void ASwarmSpawner::BeginPlay()
{
	Super::BeginPlay();

	InitializePool();

	if (bEnableAutoSpawn)
	{
		BeginSpawn();
	}
}

void ASwarmSpawner::InitializePool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (const auto& Composition : SwarmCompositions)
	{
		if (Composition.AgentClass)
		{
			if (!AgentPool.Contains(Composition.AgentClass))
			{
				AgentPool.Add(Composition.AgentClass, TArray<ASwarmAgent*>());
			}

			for (int32 i = 0; i < Composition.SpawnCount; ++i)
			{
				if (ASwarmAgent* NewAgent = World->SpawnActor<ASwarmAgent>(
					Composition.AgentClass, FVector::ZeroVector, FRotator::ZeroRotator))
				{
					NewAgent->DeactivateAgent();
					AgentPool[Composition.AgentClass].Add(NewAgent);
				}
			}
		}
	}
}

ASwarmAgent* ASwarmSpawner::GetAgentFromPool(TSubclassOf<ASwarmAgent> AgentClass)
{
	if (AgentPool.Contains(AgentClass) && AgentPool[AgentClass].Num() > 0)
	{
		return AgentPool[AgentClass].Pop();
	}

	if (UWorld* World = GetWorld())
	{
		ASwarmAgent* NewAgent = World->SpawnActor<ASwarmAgent>(AgentClass, FVector::ZeroVector, FRotator::ZeroRotator);
		return NewAgent;
	}

	return nullptr;
}

void ASwarmSpawner::ReturnAgentToPool(ASwarmAgent* Agent)
{
	if (!Agent) return;

	Agent->DeactivateAgent();

	TSubclassOf<ASwarmAgent> AgentClass = Agent->GetClass();
	if (AgentPool.Contains(AgentClass))
	{
		AgentPool[AgentClass].Add(Agent);
	}
	else
	{
		AgentPool.Add(AgentClass, TArray<ASwarmAgent*>());
		AgentPool[AgentClass].Add(Agent);
	}
}

void ASwarmSpawner::ReturnAllAgentsToPool()
{
	if (IsValid(SpawnedManager))
	{
		// 매니저가 관리하는 활성화된 모든 에이전트를 가져와서 풀에 반납
		TArray<ASwarmAgent*> ActiveAgents = SpawnedManager->GetAllActiveAgents(); // SwarmManager에 이 함수가 필요
		for (ASwarmAgent* Agent : ActiveAgents)
		{
			ReturnAgentToPool(Agent);
		}
		SpawnedManager->ClearAllAgentRefs(); // SwarmManager에서 에이전트 참조를 모두 제거하는 함수가 필요
	}
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

		if (!SpawnedManager)
		{
			SpawnedManager = World->SpawnActor<
				ASwarmManager>(SwarmManagerClass, GetActorLocation(), GetActorRotation());
		}

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

			for (const auto& Composition : SwarmCompositions)
			{
				/*
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
				*/

				if (Composition.AgentClass)
				{
					for (int32 i = 0; i < Composition.SpawnCount; ++i)
					{
						const FBox SpawnableArea = FBox(GetActorLocation() - SpawnBox->GetScaledBoxExtent(),
						                                GetActorLocation() + SpawnBox->GetScaledBoxExtent());
						const FVector SpawnLocation = FMath::RandPointInBox(SpawnableArea);
						const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

						if (ASwarmAgent* PooledAgent = GetAgentFromPool(Composition.AgentClass))
						{
							PooledAgent->AIControllerClass = Composition.ControllerClass;
							PooledAgent->MySwarmManager = SpawnedManager;

							if (UCharacterMovementComponent* MoveComp = PooledAgent->GetCharacterMovement())
							{
								MoveComp->AvoidanceWeight = Composition.AvoidanceWeight;
								MoveComp->MaxWalkSpeed = Composition.bIsLeader
									                         ? LeaderMovementSpeed
									                         : FollowerMovementSpeed;
							}

							if (Composition.bIsLeader)
							{
								if (bBeginBurserkMode)
								{
									PooledAgent->RequestBerserkMode();
								}
								PooledAgent->SetLeader(true, LeaderBehaviorTree, LeaderBlackBoard);
								SpawnedManager->SetLeader(PooledAgent);
							}

							PooledAgent->ActivateAgent(SpawnTransform);

							if (AAIController* AIController = Cast<AAIController>(PooledAgent->GetController()))
							{
								AIController->SetGenericTeamId(Composition.TeamIDToAssign);
							}

							SpawnedManager->RegisterAgent(PooledAgent);
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

void ASwarmSpawner::ResetSpawendMonster()
{
	ReturnAllAgentsToPool();
	if(IsValid(SpawnedManager))
	{
		SpawnedManager->ResetManager(); // SwarmManager에 모든 상태를 초기화하는 함수가 필요
	}
	BeginSpawn();
}

void ASwarmSpawner::DestroyAllMonster()
{
	ReturnAllAgentsToPool();

	for (auto& Elem : AgentPool)
	{
		for (ASwarmAgent* Agent : Elem.Value)
		{
			if(IsValid(Agent))
			{
				Agent->Destroy();
			}
		}
	}
	AgentPool.Empty();

	if (IsValid(SpawnedManager))
	{
		SpawnedManager->Destroy();
		SpawnedManager = nullptr;
	}
}

void ASwarmSpawner::OnMonstersUpdated_Handler(const int32 DecreaseCount)
{
	OnMonstersUpdated.Broadcast(DecreaseCount);
	UE_LOG(LogTemp, Warning, TEXT("DecreaseCount MonsterCount[%d]"), DecreaseCount);
}
