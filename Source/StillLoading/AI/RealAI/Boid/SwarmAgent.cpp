#include "SwarmAgent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SwarmManager.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BoidMovementComponent/BoidMovementComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/RadarComponent/CollisionRadarComponent.h"
#include "Kismet/GameplayStatics.h"

ASwarmAgent::ASwarmAgent()
{
	PrimaryActorTick.bCanEverTick = false;

	CachedBoidMovementComp = CreateDefaultSubobject<UBoidMovementComponent>("BoidMovement");
}

FGenericTeamId ASwarmAgent::GetGenericTeamId() const
{
	if (const AController* MyController = GetController())
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(MyController))
		{
			return TeamAgent->GetGenericTeamId();
		}
	}

	return FGenericTeamId::NoTeam;
}

void ASwarmAgent::BeginPlay()
{
	Super::BeginPlay();

	CachedRadarComponent = FindComponentByClass<UCollisionRadarComponent>();

	if (MySwarmManager && CachedRadarComponent && bIsLeader)
	{
		CachedRadarComponent->ToggleRadarComponent(true, MySwarmManager->DetectionRadius);
		CachedRadarComponent->OnActorDetectedEnhanced.
							  AddDynamic(this, &ASwarmAgent::OnRadarDetectedActor);
	}
}

void ASwarmAgent::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AAIController* AIController = Cast<AAIController>(NewController))
	{
		ApplyLeaderState(AIController);

		if (bShouldEnterBerserkOnPossess)
		{
			ApplyBerserkState();
		}
	}
}

void ASwarmAgent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASwarmAgent::RequestBerserkMode()
{
	bShouldEnterBerserkOnPossess = true;
}

ASwarmManager* ASwarmAgent::GetMySwarmManager() const
{
	return MySwarmManager;
}

void ASwarmAgent::ApplyBerserkState()
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BBComp = AICon->GetBlackboardComponent())
		{
			ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			BBComp->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
			UE_LOG(LogTemp, Warning, TEXT("%s is now in BERSERK mode. Target locked."), *GetName());
		}
	}

	if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
	{
		Monster->SetMonsterModeState(TAG_AI_Berserk);
	}
}

// TODO:: 추후에 강제적으로 리더를 불러오는 로직 필요
void ASwarmAgent::Hited(AActor* Causer)
{
	AAIController* LeaderController = Cast<AAIController>(GetController());
	UBlackboardComponent* BlackboardComp = LeaderController->GetBlackboardComponent();
	if (!BlackboardComp) return;
	BlackboardComp->SetValueAsObject(BlackboardKeys::TargetActor, Causer);
}

void ASwarmAgent::SetLeader(bool IsLeader, UBehaviorTree* LeaderBehaviorTree, UBlackboardData* LeaderBlackBoard)
{
	if (bIsLeader == IsLeader)
	{
		return;
	}

	bIsLeader = IsLeader;
	AgentID = bIsLeader ? -1 : 0;

	if (bIsLeader && LeaderBehaviorTree && LeaderBlackBoard)
	{
		CachedLeaderBehaviorTree = LeaderBehaviorTree;
		CachedLeaderBlackboard = LeaderBlackBoard;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		ApplyLeaderState(AIController);
	}
}

void ASwarmAgent::OnRadarDetectedActor(AActor* DetectedActor, float Distance)
{
	if (APawn* CastedActor = Cast<APawn>(DetectedActor))
	{
		CurrentDetectedActor = CastedActor;
	}
}

void ASwarmAgent::ApplyLeaderState(AAIController* AIController)
{
	if (!AIController) return;

	if (AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Applying new leader state");
	}

	if (bIsLeader)
	{
		if (CachedLeaderBehaviorTree && CachedLeaderBlackboard)
		{
			UBlackboardComponent* BlackboardComponent = nullptr;
			AIController->UseBlackboard(CachedLeaderBlackboard, BlackboardComponent);

			AIController->RunBehaviorTree(CachedLeaderBehaviorTree);
			UE_LOG(LogTemp, Warning, TEXT("%s starts logic as a LEADER."), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s stops logic as a FOLLOWER."), *GetName());
	}
}

void ASwarmAgent::AgentDied()
{
	FOnMonsterDied.Broadcast(this);
}

void ASwarmAgent::PlayAttackAnim()
{
	if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
	{
		if (Monster->HasStrategyState(TAG_AI_IsPlayingMontage)) return;

		Monster->SetPrimaryState(TAG_AI_IsAttacking);
		Monster->SetStrategyState(TAG_AI_IsPlayingMontage);
	}

	if (UAnimationMontageComponent* AnimComp = FindComponentByClass<UAnimationMontageComponent>())
	{
		int8 RandNum = FMath::RandRange(0, 2);
		if (RandNum == 0)
		{
			AnimComp->PlayAIAttackMontage("Attack1");
		}
		else if (RandNum == 1)
		{
			AnimComp->PlayAIAttackMontage("Attack2");
		}
		else
		{
			AnimComp->PlayAIAttackMontage("Attack3");
		}
	}
}

void ASwarmAgent::PlayETCAnim()
{
	if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
	{
		if (Monster->HasStrategyState(TAG_AI_IsPlayingMontage)) return;

		Monster->SetStrategyState(TAG_AI_IsPlayingMontage);
	}

	if (UAnimationMontageComponent* AnimComp = FindComponentByClass<UAnimationMontageComponent>())
	{
		int8 RandNum = FMath::RandRange(0, 4);
		if (RandNum == 0)
		{
			AnimComp->PlayAIETCMontage("WonderA");
		}
		else if (RandNum == 1)
		{
			AnimComp->PlayAIETCMontage("WonderB");
		}
		else if (RandNum == 2)
		{
			AnimComp->PlayAIETCMontage("WonderC");
		}
		else if (RandNum == 3)
		{
			AnimComp->PlayAIETCMontage("WonderD");
		}
		else if (RandNum == 4)
		{
			AnimComp->PlayAIETCMontage("WonderE");
		}
	}
}
