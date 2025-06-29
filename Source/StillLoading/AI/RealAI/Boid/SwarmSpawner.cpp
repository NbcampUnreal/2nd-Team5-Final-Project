#include "SwarmSpawner.h"

#include "NiagaraFunctionLibrary.h"
#include "SwarmAgent.h"
#include "SwarmManager.h"
#include "AnimInstances/SLCompanionAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void ASwarmSpawner::RespawnSingleAgent(TSubclassOf<ASwarmAgent> AgentClassToRespawn)
{
	 if (!SpawnedManager || !AgentClassToRespawn)
    {
        return;
    }
    
    if (ASwarmAgent* PooledAgent = GetAgentFromPool(AgentClassToRespawn))
    {
        const FBox SpawnableArea = FBox(GetActorLocation() - SpawnBox->GetScaledBoxExtent(),
                                        GetActorLocation() + SpawnBox->GetScaledBoxExtent());
        const FVector SpawnLocation = FMath::RandPointInBox(SpawnableArea);
        const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

        const FSwarmComposition* FoundComposition = SwarmCompositions.FindByPredicate(
            [&](const FSwarmComposition& Comp)
            {
                return Comp.AgentClass == AgentClassToRespawn;
            });

        if (!FoundComposition)
        {
            ReturnAgentToPool(PooledAgent);
            UE_LOG(LogTemp, Warning, TEXT("Could not find composition for respawning agent of class %s"), *AgentClassToRespawn->GetName());
            return;
        }

        if(FoundComposition->bIsLeader)
        {
             ReturnAgentToPool(PooledAgent);
             return;
        }

        PooledAgent->AIControllerClass = FoundComposition->ControllerClass;
        PooledAgent->MySwarmManager = SpawnedManager;

        if (UCharacterMovementComponent* MoveComp = PooledAgent->GetCharacterMovement())
        {
            MoveComp->AvoidanceWeight = FoundComposition->AvoidanceWeight;
            MoveComp->MaxWalkSpeed = FollowerMovementSpeed;
        }

        PooledAgent->ActivateAgent(SpawnTransform);

        if (AAIController* AIController = Cast<AAIController>(PooledAgent->GetController()))
        {
            AIController->SetGenericTeamId(FoundComposition->TeamIDToAssign);
        }

        SpawnedManager->RegisterAgent(PooledAgent);

        UE_LOG(LogTemp, Log, TEXT("An agent of class %s has been respawned."), *AgentClassToRespawn->GetName());
    }
}

void ASwarmSpawner::ReturnAllAgentsToPool()
{
	if (IsValid(SpawnedManager))
	{
		TArray<ASwarmAgent*> ActiveAgents = SpawnedManager->GetAllActiveAgents();
		for (ASwarmAgent* Agent : ActiveAgents)
		{
			ReturnAgentToPool(Agent);
		}
		SpawnedManager->ClearAllAgentRefs();
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
			SpawnedManager->MySpawner = this;

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

			SpawnedManager->CachedLeaderBT = LeaderBehaviorTree;
			SpawnedManager->CachedLeaderBB = LeaderBlackBoard;

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
	if (IsValid(SpawnedManager))
	{
		SpawnedManager->ResetManager();
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
			if (IsValid(Agent))
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
