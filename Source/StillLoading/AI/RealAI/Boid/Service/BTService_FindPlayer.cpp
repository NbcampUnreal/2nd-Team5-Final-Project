#include "BTService_FindPlayer.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = "Find Player";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!CheckTag(OwnerComp)) return;

	const APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;
	ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(AIController->GetPawn());
	if (!OwningAgent) return;
	ASwarmManager* SwarmManager = OwningAgent->GetMySwarmManager();
	if (!ControlledPawn || !BlackboardComp || !SwarmManager) return;

	if (const AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(OwningAgent))
	{
		if (Monster->HasMonsterModeState(TAG_AI_Berserk))
		{
			ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
			return;
		}
	}

	const float DistanceToTarget = OwningAgent->GetDistanceTo(OwningAgent->CurrentDetectedActor);

	if (DistanceToTarget <= SwarmManager->DetectionRadius)
	{
		IGenericTeamAgentInterface* OwningTeamAgent = Cast<IGenericTeamAgentInterface>(OwningAgent);
		IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(
			OwningAgent->CurrentDetectedActor);

		if (OwningTeamAgent && TargetTeamAgent)
		{
			if (OwningTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId())
			{
				SwarmManager->ReportTargetSighting(OwningAgent, OwningAgent->CurrentDetectedActor);

				BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, OwningAgent->CurrentDetectedActor);
				BlackboardComp->SetValueAsFloat(TEXT("LastSeenTime"), GetWorld()->GetTimeSeconds());
				SwarmManager->SetSquadState(ESquadState::Engaging);
			}
			else
			{
				OwningAgent->CurrentDetectedActor = nullptr;
			}
		}
	}
	else
	{
		if (BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName) != nullptr
			|| OwningAgent->CurrentDetectedActor != nullptr)
		{
			const float LastSeenTime = BlackboardComp->GetValueAsFloat(TEXT("LastSeenTime"));
			const float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

			if (TimeSinceLastSeen > ForgettingTime)
			{
				BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
				SwarmManager->SetSquadState(ESquadState::Patrolling_Move);
				OwningAgent->CurrentDetectedActor = nullptr;
				//UE_LOG(LogTemp, Warning, TEXT("Target LOST. Clearing TargetActor."));
			}
		}
	}
}

bool UBTService_FindPlayer::CheckTag(const UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) { return false; }

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) { return false; }

	if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(ControlledPawn))
	{
		if (Monster->HasMonsterModeState(TAG_AI_Berserk)) return false;
	}

	return true;
}
