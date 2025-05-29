#include "MonsterAIController.h"

#include "EngineUtils.h"
#include "AI/Testing/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 45.0f;
	SightConfig->SetMaxAge(5.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerception);

	AIPerception->OnPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnPerceptionUpdated);
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerceptionUpdated);
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(1));

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
		BlackboardComponent = GetBlackboardComponent();
	}

	if (Blackboard)
	{
		Blackboard->SetValueAsObject("SelfActor", GetPawn());
	}
}

void AMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 개별 액터의 감지/미감지 상태 기반 행동(추격 시작/중단 등)을 할 때
void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (ASLPlayerCharacterBase* Player = Cast<ASLPlayerCharacterBase>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			BlackboardComponent->SetValueAsObject("TargetActor", Actor);
			if (AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn()))
			{
				MyChar->SetChasing(true);
				MyChar->SetPrimaryState(TAG_AI_AbleToAttack);

				AlertNearbyAllies(Player);
			}
		}
		else
		{
			BlackboardComponent->ClearValue("TargetActor");
			if (AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn()))
			{
				MyChar->SetChasing(false);
				MyChar->SetPrimaryState(TAG_AI_Idle);
			}
		}
	}
}

void AMonsterAIController::AlertNearbyAllies(AActor* TargetActor, float AlertRadius)
{
	if (!TargetActor || !GetPawn()) return;

	const FVector MyLocation = GetPawn()->GetActorLocation();
	const FGenericTeamId MyTeamId = GetGenericTeamId();

	for (TActorIterator<AMonsterAICharacter> It(GetWorld()); It; ++It)
	{
		const AMonsterAICharacter* Ally = *It;
		if (!Ally || Ally == GetPawn()) continue;

		const float Distance = FVector::Dist(MyLocation, Ally->GetActorLocation());
		if (Distance > AlertRadius) continue;

		AAIController* AllyController = Cast<AAIController>(Ally->GetController());
		if (!AllyController) continue;

		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(AllyController))
		{
			if (TeamAgent->GetGenericTeamId() != MyTeamId) continue;
		}

		if (UBlackboardComponent* BB = AllyController->GetBlackboardComponent())
		{
			if (!BB->GetValueAsObject("TargetActor"))
			{
				BB->SetValueAsObject("TargetActor", TargetActor);
			}
		}
	}
}

// 감지 리스트를 기반으로 우선 타겟 선정, 시야각 등 전역 판단할 때
void AMonsterAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (ASLPlayerCharacterBase* Player = Cast<ASLPlayerCharacterBase>(Actor))
		{
			if (LineOfSightTo(Player))
			{
				//BlackboardComponent->SetValueAsObject("TargetActor", Player);
				//UE_LOG(LogTemp, Warning, TEXT("전체 감지 업데이트: %s"), *Player->GetName());
				break;
			}
		}
	}
}

ETeamAttitude::Type AMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
	FGenericTeamId MyTeamID = GetGenericTeamId();

	// 팀 ID가 같으면 우호적
	if (OtherTeamID == MyTeamID)
	{
		return ETeamAttitude::Friendly;
	}
	else
	{
		return ETeamAttitude::Hostile;
	}
}
