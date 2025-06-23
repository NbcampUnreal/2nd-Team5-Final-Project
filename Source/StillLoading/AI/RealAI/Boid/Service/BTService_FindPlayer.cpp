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
	const ASwarmAgent* OwningAgent = Cast<ASwarmAgent>(AIController->GetPawn());
	if (!OwningAgent) return;
	ASwarmManager* SwarmManager = OwningAgent->GetMySwarmManager();
	if (!ControlledPawn || !BlackboardComp || !SwarmManager) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
		return;
	}

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	if (Distance < SwarmManager->DetectionRadius)
	{
		BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);
		BlackboardComp->SetValueAsFloat(TEXT("LastSeenTime"), GetWorld()->GetTimeSeconds());
		SwarmManager->SetSquadState(ESquadState::Engaging);
	}
	else
	{
		if (BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName) != nullptr)
		{
			const float LastSeenTime = BlackboardComp->GetValueAsFloat(TEXT("LastSeenTime"));
			const float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

			if (TimeSinceLastSeen > ForgettingTime)
			{
				BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
				SwarmManager->SetSquadState(ESquadState::Patrolling_Move);
				UE_LOG(LogTemp, Warning, TEXT("Target LOST. Clearing TargetActor."));
			}
		}
	}
}

bool UBTService_FindPlayer::IsInFieldOfView(const AActor* TargetActor, const APawn* ControlledPawn, const ASwarmManager* SwarmManager)
{
	if (!TargetActor)
		return false;

	FVector MyLocation = ControlledPawn->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	float Distance = FVector::Dist(MyLocation, TargetLocation);
	if (Distance > SwarmManager->DetectionRadius)
		return false;

	FVector ForwardVector = ControlledPawn->GetActorForwardVector().GetSafeNormal();
	FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

	float DotProduct = FVector::DotProduct(ForwardVector, ToTarget);
	float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(100 * 0.5f));

	return DotProduct >= CosHalfFOV;
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
