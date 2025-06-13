#include "AISquadManager.h"

#include "AIController.h"
#include "Blackboardkeys.h"
#include "FormationComponent.h"
#include "MonsterAICharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AAISquadManager::AAISquadManager()
{
	PrimaryActorTick.bCanEverTick = true;

	FormationComponent = CreateDefaultSubobject<UFormationComponent>(TEXT("FormationComponent"));
}

void AAISquadManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAISquadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInBerserkMode) return;

	if (!IsValid(Leader))
	{
		bIsInBerserkMode = true; 

		if (SharedBlackboard)
		{
			AActor* FinalTarget = Cast<AActor>(SharedBlackboard->GetValueAsObject(BlackboardKeys::TargetActor));

			if (!FinalTarget)
			{
				if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
				{
					SharedBlackboard->SetValueAsObject(BlackboardKeys::TargetActor, PlayerPawn);
					FinalTarget = PlayerPawn;
				}
			}

			if (FinalTarget)
			{
				for (TObjectPtr<AActor> Follower : SquadFollowers)
				{
					if (AMonsterAICharacter* MonsterFollower = Cast<AMonsterAICharacter>(Follower))
					{
						MonsterFollower->SetStrategyState(TAG_JOBMOB_ATTACK);
					}
				}
			}
		}
        
		return;
	}

	if (const AActor* CurrentTarget = Cast<AActor>(SharedBlackboard->GetValueAsObject(BlackboardKeys::TargetActor)))
	{
		bool bIsTargetLost = false;
		if (!IsValid(CurrentTarget))
		{
			bIsTargetLost = true;
		}
		else
		{
			const float DistanceToTarget = FVector::Dist(Leader->GetActorLocation(), CurrentTarget->GetActorLocation());
			if (DistanceToTarget > TargetDetectionRadius)
			{
				bIsTargetLost = true;
			}
		}
        
		if (bIsTargetLost)
		{
			SharedBlackboard->ClearValue(BlackboardKeys::TargetActor);

			for (TObjectPtr<AActor> Agent : AttackingAgents)
			{
				if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(Agent))
				{
					Monster->SetStrategyState(TAG_JOBMOB_HOLDPOSITION);
				}
			}

			AttackingAgents.Empty();
		}
	}
	else 
	{
		OrderPatrol(); 

		if (bIsPatrolling)
		{
			Patrol();
		}
	}

	EFormationType FormationType = EFormationType::Circle;

	if (AActor* CurrentTarget = Cast<AActor>(SharedBlackboard->GetValueAsObject(BlackboardKeys::TargetActor)))
	{
		FormationType = EFormationType::BackCircle;

		if (FormationComponent)
		{
			FormationComponent->AssignStoredFormation(CurrentTarget, SquadFollowers, AttackingAgents, FormationType, 400.f);
		}
	}
}

void AAISquadManager::InitializeSquad(const TArray<AActor*>& AllMembers)
{
	if (AllMembers.IsEmpty()) return;

	Leader = AllMembers[0];
	SquadFollowers.Empty();

	for (int i = 1; i < AllMembers.Num(); ++i)
	{
		SquadFollowers.Add(AllMembers[i]);
		if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(AllMembers[i]))
		{
			Monster->SetSquadManager(this);
		}
	}
	
	if (AMonsterAICharacter* MonsterLeader = Cast<AMonsterAICharacter>(Leader))
	{
		MonsterLeader->SetSquadManager(this);
	}

	if (Leader)
	{
		if (const AMonsterAICharacter* MonsterLeader = Cast<AMonsterAICharacter>(Leader))
		{
			if (AAIController* AICon = Cast<AAIController>(MonsterLeader->GetController()))
			{
				LeaderController = AICon;
				SharedBlackboard = AICon->GetBlackboardComponent();
			}
		}
	}
}

void AAISquadManager::Order(EOrderType OrderType)
{
	if (FormationComponent) FormationComponent->StopFidgeting();
	bIsPatrolling = false;
	if (LeaderController)
	{
		LeaderController->StopMovement();
	}
	
	CleanUpInvalidAgents();
	switch (OrderType)
	{
	case EOrderType::Attack:
		Attack();
		break;
	case EOrderType::HoldPosition:
		HoldPosition(true);
		break;
	}
}

void AAISquadManager::OrderPatrol()
{
	if (!Leader || !LeaderController || !SharedBlackboard) return;
	if (FormationComponent) FormationComponent->StopFidgeting();

	if (bIsPatrolling) return;
	
	if (SharedBlackboard->GetValueAsObject(BlackboardKeys::TargetActor) != nullptr) return;

	bIsPatrolling = true;
	CurrentPatrolDestination = FVector::ZeroVector;
}

void AAISquadManager::Attack()
{
	CleanUpInvalidAgents();
	
	if (SquadFollowers.IsEmpty() || AttackingAgents.Num() >= 2) return;

	const int32 NumToSelect = FMath::Min(2, SquadFollowers.Num());
    
	if (NumToSelect <= 0)
	{
		for (TObjectPtr<AActor> StoredAgent : SquadFollowers)
		{
			if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(StoredAgent))
			{
				Monster->SetStrategyState(TAG_JOBMOB_ATTACK);
			}
		}
		return;
	}

	TArray<int32> Indices;
	Indices.Reserve(SquadFollowers.Num());
	for (int32 i = 0; i < SquadFollowers.Num(); ++i)
	{
		Indices.Add(i);
	}

	for (int32 i = Indices.Num() - 1; i > 0; --i)
	{
		const int32 j = FMath::RandRange(0, i);
		Indices.Swap(i, j);
	}

	for (int32 i = 0; i < NumToSelect; ++i)
	{
		const int32 AgentIndex = Indices[i];

		if (AActor* StoredAgent = SquadFollowers[AgentIndex].Get())
		{
			if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(StoredAgent))
			{
				Monster->SetStrategyState(TAG_JOBMOB_ATTACK);
				AttackingAgents.Add(StoredAgent);
			}
		}
	}
}

void AAISquadManager::HoldPosition(bool bIsAll)
{
	TArray<TObjectPtr<AActor>> AgentsToHold; 

	for (TObjectPtr<AActor> Agent : SquadFollowers)
	{
		if (!bIsAll)
		{
			if (AttackingAgents.Contains(Agent)) continue;
		}
       
		if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(Agent))
		{
			Monster->SetStrategyState(TAG_JOBMOB_HOLDPOSITION);
			AgentsToHold.Add(Agent.Get());
		}
	}

	if (FormationComponent)
	{
		FormationComponent->StartFidgetingFor(AgentsToHold);
	}
}

void AAISquadManager::CleanUpInvalidAgents()
{
	SquadFollowers.RemoveAll([](const TObjectPtr<AActor>& Agent)
	{
		return !IsValid(Agent.Get());
	});

	AttackingAgents.RemoveAll([](const TObjectPtr<AActor>& Agent)
	{
		return !IsValid(Agent.Get());
	});
}

void AAISquadManager::Patrol()
{
	if (!Leader || !LeaderController || !SharedBlackboard)
	{
		bIsPatrolling = false;
		return;
	}

	const FVector LeaderLocation = Leader->GetActorLocation();
	const float DebugLifeTime = 5.0f;

	//DrawDebugSphere(GetWorld(), LeaderLocation, TargetDetectionRadius, 16, FColor::Red, false, 0.1f, 0, 1.0f);
	
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    
	UKismetSystemLibrary::SphereOverlapActors(
	   GetWorld(),
	   Leader->GetActorLocation(),
	   TargetDetectionRadius,
	   ObjectTypes,
	   ASLPlayerCharacter::StaticClass(),
	   {},
	   OverlappedActors
	);

	if (!OverlappedActors.IsEmpty())
	{
		AActor* FoundTarget = OverlappedActors[0];
		SharedBlackboard->SetValueAsObject(TEXT("TargetActor"), FoundTarget);
       
		bIsPatrolling = false;
		LeaderController->StopMovement();
		//DrawDebugLine(GetWorld(), LeaderLocation, FoundTarget->GetActorLocation(), FColor::Magenta, false, DebugLifeTime, 0, 2.0f);
		
		return;
	}
    
	const bool bHasNoDestination = CurrentPatrolDestination.IsZero();
	const float DistanceToDestination = FVector::Dist(LeaderLocation, CurrentPatrolDestination);

	if (bHasNoDestination || DistanceToDestination < PatrolDestinationReachedTolerance)
	{
		if (const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation NewDestination;
			if (NavSystem->GetRandomReachablePointInRadius(LeaderLocation, PatrolRadiusFromLeader, NewDestination))
			{
				CurrentPatrolDestination = NewDestination.Location;
				//DrawDebugLine(GetWorld(), LeaderLocation, CurrentPatrolDestination, FColor::Cyan, false, 5.0f, 0, 2.0f);
			}
		}
	}

	LeaderController->MoveToLocation(CurrentPatrolDestination);

	if (FormationComponent)
	{
		FormationComponent->AssignStoredFormation(Leader, SquadFollowers, AttackingAgents, EFormationType::Circle, 200.f);
	}
}

