#include "BTService_FindPlayer.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "GameFramework/Character.h"
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

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ControlledPawn || !BlackboardComp) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
		return;
	}

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	if (Distance < DetectionRadius)
	{
		BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);
		BlackboardComp->SetValueAsFloat(TEXT("LastSeenTime"), GetWorld()->GetTimeSeconds());
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
				UE_LOG(LogTemp, Warning, TEXT("Target LOST. Clearing TargetActor."));
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
