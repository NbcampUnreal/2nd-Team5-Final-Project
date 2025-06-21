#include "BTService_FindPlayer.h"

#include "AIController.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ControlledPawn || !BlackboardComp) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return;

	if (FVector::Dist(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation()) < DetectionRadius)
	{
		BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerCharacter);
		BlackboardComp->SetValueAsFloat(TEXT("LastSeenTime"), GetWorld()->GetTimeSeconds());

		if (bDoOnce) return;
		if (ASwarmManager* Manager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass())))
		{
			Manager->SetSquadState(ESquadState::Engaging);
			bDoOnce = true;
		}
	}
	else
	{
		if (BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName) != nullptr)
		{
			float LastSeenTime = BlackboardComp->GetValueAsFloat(TEXT("LastSeenTime"));
			float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

			if (TimeSinceLastSeen > ForgettingTime)
			{
				BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
			}
		}
	}
}