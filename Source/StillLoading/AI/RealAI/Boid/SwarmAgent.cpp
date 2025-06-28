#include "SwarmAgent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SwarmManager.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BoidMovementComponent/BoidMovementComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/RadarComponent/CollisionRadarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASwarmAgent::ASwarmAgent()
{
	PrimaryActorTick.bCanEverTick = true;

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

void ASwarmAgent::StartSpinning(float Degrees)
{
	DegreesPerSecond = Degrees;
	bIsSpinning = true;
}

void ASwarmAgent::StopSpinning()
{
	bIsSpinning = false;
}

void ASwarmAgent::BeginPlay()
{
	Super::BeginPlay();

	CachedRadarComponent = FindComponentByClass<UCollisionRadarComponent>();

	if (MySwarmManager && CachedRadarComponent)
	{
		//if (!IsLeader()) return;
		CachedRadarComponent->ToggleRadarComponent(true, MySwarmManager->DetectionRadius);
		CachedRadarComponent->OnActorDetectedEnhanced.
		                      AddDynamic(this, &ASwarmAgent::OnRadarDetectedActor);

		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ASwarmAgent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsSpinning)
	{
		if (const AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
		{
			if (Monster->HasBattleState(TAG_AI_Dead))
			{
				bIsSpinning = false;
				return;
			}
		}
		
		const FRotator DeltaRotation = FRotator(0.f, DegreesPerSecond * DeltaSeconds, 0.f);
		AddActorLocalRotation(DeltaRotation);
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
	if (CurrentDetectedActor == DetectedActor) return;
	
	if (APawn* CastedActor = Cast<APawn>(DetectedActor))
	{
		CurrentDetectedActor = CastedActor;

		const float DistanceToTarget = GetDistanceTo(CurrentDetectedActor);

		if (DistanceToTarget <= MySwarmManager->DetectionRadius)
		{
			IGenericTeamAgentInterface* OwningTeamAgent = Cast<IGenericTeamAgentInterface>(this);
			IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(CurrentDetectedActor);

			if (OwningTeamAgent && TargetTeamAgent)
			{
				if (OwningTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId())
				{
					MySwarmManager->ReportTargetSighting(this, CastedActor);
				}
			}
		}
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

void ASwarmAgent::RotateToFaceTarget()
{
	if (!MySwarmManager) return;

	if (const AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
	{
		if (Monster->HasBattleState(TAG_AI_Dead)) return;
	}
	
	const AActor* TargetActor = MySwarmManager->CurrentSquadTarget;

	if (!TargetActor)
	{
		return;
	}

	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	FVector Direction = (TargetLocation - MyLocation).GetSafeNormal();

	Direction.Z = 0.0f;

	const FRotator TargetRotation = Direction.Rotation();

	SetActorRotation(TargetRotation);
}

void ASwarmAgent::PlayAttackAnim()
{
	RotateToFaceTarget();
	
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
	RotateToFaceTarget();
	
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

void ASwarmAgent::PlayETCWaitAnim()
{
	if (AMonsterAICharacter* Monster = Cast<AMonsterAICharacter>(this))
	{
		if (Monster->HasStrategyState(TAG_AI_IsPlayingMontage)
			|| Monster->HasBattleState(TAG_AI_Dead)) return;
		
		Monster->SetStrategyState(TAG_AI_IsPlayingMontage);
	}

	if (UAnimationMontageComponent* AnimComp = FindComponentByClass<UAnimationMontageComponent>())
	{
		int8 RandNum = FMath::RandRange(0, 3);
		if (RandNum == 0)
		{
			StartSpinning(90);
			AnimComp->PlayAIETCMontage("WaitA");
		}
		else if (RandNum == 1)
		{
			StartSpinning(180);
			AnimComp->PlayAIETCMontage("WaitB");
		}
		else if (RandNum == 2)
		{
			StartSpinning(-90);
			AnimComp->PlayAIETCMontage("WaitC");
		}
		else
		{
			StartSpinning(-180);
			AnimComp->PlayAIETCMontage("WaitD");
		}
	}
}
