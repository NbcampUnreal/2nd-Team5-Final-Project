#include "SwarmManager.h"

#include "NavigationSystem.h"
#include "SwarmAgent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavFilters/NavigationQueryFilter.h"

ASwarmManager::ASwarmManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASwarmManager::DestroyAllAgents()
{
    for (ASwarmAgent* Agent : AllAgents)
    {
        if (IsValid(Agent))
        {
            Agent->Destroy();
        }
    }
}

void ASwarmManager::SetNewPath(const TArray<FVector>& NewPathPoints)
{
	CurrentPath = NewPathPoints;
}

// 에이전트 등록 & 해제
void ASwarmManager::RegisterAgent(ASwarmAgent* Agent)
{
	if (Agent)
	{
		AllAgents.Add(Agent);
		Agent->SetAgentID(NextAgentID++);
		Agent->FOnMonsterDied.AddDynamic(this, &ASwarmManager::AgentDead);
	}

	if (Agent && Agent->GetAgentID() >= 0)
	{
		const FVector RandomOffset = FVector(
			FMath::RandRange(-MaxRandomOffset, MaxRandomOffset),
			FMath::RandRange(-MaxRandomOffset, MaxRandomOffset),
			0.0f
		);
		AgentRandomOffsets.Add(Agent->GetAgentID(), RandomOffset);
	}

	TotalSpawnCount++;
}

void ASwarmManager::UnregisterAgent(ASwarmAgent* Agent)
{
	if (Agent)
	{
		AllAgents.Remove(Agent);
	}

	if (Agent && Agent->GetAgentID() >= 0)
	{
		AgentRandomOffsets.Remove(Agent->GetAgentID());
	}

	if (OnMonstersUpdated.IsBound())
	{
		//int32 DecreaseCount = TotalSpawnCount - AllAgents.Num();
		OnMonstersUpdated.Broadcast(1);
		//LastMonsterCount = AllAgents.Num();
	}
}

// 대열셋팅 (안하면 벽비빔)
FVector ASwarmManager::GetFormationSlotLocationForAgent(int32 AgentID) const
{
    ASwarmAgent* Leader = GetLeader();
    if (!Leader || AgentID < 0) return FVector::ZeroVector;

    const int32 Row = AgentID / NumColumns;
    const int32 Column = AgentID % NumColumns;

    FVector IdealSlotLocation = FVector::ZeroVector;

    switch (CurrentFormationType)
    {
        case EFollowerFormationType::BehindLeader:
        {
            const float ColumnOffset = static_cast<float>(Column) - (static_cast<float>(NumColumns - 1) / 2.0f);
            
            const float BehindDistance = FormationOffsetBehindLeader + (Row * RowSpacing);
            const float SideDistance = ColumnOffset * ColumnSpacing;

            IdealSlotLocation = Leader->GetActorLocation()
                               - (Leader->GetActorForwardVector() * BehindDistance)
                               + (Leader->GetActorRightVector() * SideDistance);
            break;
        }
    	
        case EFollowerFormationType::CenteredSquare:
        {
            const float ColumnOffset = static_cast<float>(Column) - (static_cast<float>(NumColumns - 1) / 2.0f);
            const float RowOffset = static_cast<float>(Row) - (static_cast<float>(NumRows - 1) / 2.0f);
            
            const float SideDistance = ColumnOffset * ColumnSpacing;
            const float ForwardDistance = RowOffset * RowSpacing;

            IdealSlotLocation = Leader->GetActorLocation()
                               - (Leader->GetActorForwardVector() * ForwardDistance)
                               + (Leader->GetActorRightVector() * SideDistance);

            if (const FVector* StoredOffset = AgentRandomOffsets.Find(AgentID))
            {
                IdealSlotLocation += *StoredOffset;
            }
            break;
        }
    }
	
    if (bShowFollowersDebugLine)
    {
       DrawDebugSphere(GetWorld(), IdealSlotLocation, 50.f, 12, FColor::Yellow, false, 0.1f, 0, 3.f);
    }

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Leader);

	TArray<AActor*> IgnoredActors;
	for (const TObjectPtr<ASwarmAgent>& AgentPtr : AllAgents)
	{
		if (AgentPtr)
		{
			IgnoredActors.Add(AgentPtr);
		}
	}
    Params.AddIgnoredActors(IgnoredActors);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
       HitResult,
       Leader->GetActorLocation(),
       IdealSlotLocation,
       ECollisionChannel::ECC_Visibility,
       Params
    );

    if (bShowFollowersDebugLine)
    {
       DrawDebugLine(GetWorld(), Leader->GetActorLocation(), IdealSlotLocation, bHit ? FColor::Red : FColor::Green, false, 0.1f, 0, 3.f);
    }

    const FVector FinalTargetLocation = IdealSlotLocation;
	
    if (bHit)
    {
        // 만약 장애물에 막혔을 경우, 다른 위치를 지정하고 싶다면 이 안에 로직을 구현합니다.
    }

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation ProjectedLocation;
    if (NavSys && NavSys->ProjectPointToNavigation(FinalTargetLocation, ProjectedLocation, FVector(200.f, 200.f, 200.f)))
    {
        if (bShowFollowersDebugLine)
        {
           DrawDebugSphere(GetWorld(), ProjectedLocation.Location, 60.f, 12, FColor::Purple, false, 0.1f, 0, 4.f);
        }
        
        return ProjectedLocation.Location;
    }

    if (bShowFollowersDebugLine)
    {
       DrawDebugSphere(GetWorld(), FinalTargetLocation, 60.f, 12, FColor::Orange, false, 0.1f, 0, 4.f);
    }

    return FinalTargetLocation;
}

void ASwarmManager::SetSquadState(const ESquadState NewState)
{
	CurrentSquadState = NewState;
}

void ASwarmManager::AgentDead(ASwarmAgent* Agent)
{
	if (!Agent) return;

	const bool bWasLeader = (Agent == LeaderAgent);
	UnregisterAgent(Agent);

	if (bWasLeader)
	{
		TryToAppointNewLeader();
	}
}

void ASwarmManager::TryToAppointNewLeader()
{
	LeaderAgent = nullptr;

	if (AllAgents.Num() > 0)
	{
		if (ASwarmAgent* NewLeader = AllAgents[0])
		{
			NewLeader->SetLeader(true);

			if (AAIController* NewLeaderController = Cast<AAIController>(NewLeader->GetController()))
			{
				if (UBlackboardComponent* NewLeaderBlackboard = NewLeaderController->GetBlackboardComponent())
				{
					if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
					{
						NewLeaderBlackboard->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
						UE_LOG(LogTemp, Log, TEXT("TargetActor has been set to the new leader's blackboard."));
					}
				}
			}
            
			SetLeader(NewLeader); 

			UE_LOG(LogTemp, Warning, TEXT("SwarmManager: %s is the new leader!"), *NewLeader->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SwarmManager: No agents left to be a leader."));
	}
}