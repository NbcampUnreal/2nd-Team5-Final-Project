#include "MonsterAIController.h"

#include "MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->OnPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnPerceptionUpdated);
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

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

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonsterAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (LineOfSightTo(Actor))
		{
			BlackboardComponent->SetValueAsObject("TargetActor", Actor);
			AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn());
			if (MyChar)
			{
				MyChar->SetChasing(true);
				MyChar->SetPrimaryState(TAG_AI_AbleToAttack);
			}
			bFoundValidTarget = true;
			break;
		}
	}

	// 감지 대상 없음
	if (!bFoundValidTarget && BlackboardComponent)
	{
		BlackboardComponent->ClearValue("TargetActor");
		if (AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn()))
		{
			MyChar->SetChasing(false);
			MyChar->SetPrimaryState(TAG_AI_Idle);
		}
	}
}

