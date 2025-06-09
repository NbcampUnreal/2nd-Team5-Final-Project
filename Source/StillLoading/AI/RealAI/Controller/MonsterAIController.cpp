#include "MonsterAIController.h"

#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

struct FStateTreeInstanceData;

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// Sight 구성
	SightConfig->SightRadius = 3500.0f;
	SightConfig->LoseSightRadius = 3700.0f;
	SightConfig->PeripheralVisionAngleDegrees = 120.0f;
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
		Blackboard->SetValueAsObject(BlackboardKeys::SelfActor, GetPawn());
	}

	if (InPawn)
	{
		ACharacter* AICharacter = Cast<ACharacter>(InPawn);
		if (AICharacter && AICharacter->GetMesh())
		{
			LockOnWidgetFront = NewObject<UWidgetComponent>(AICharacter);
			LockOnWidgetFront->SetupAttachment(AICharacter->GetMesh());
			LockOnWidgetFront->RegisterComponent();
			
			LockOnWidgetFront->SetWidgetSpace(EWidgetSpace::World);
			LockOnWidgetFront->SetDrawAtDesiredSize(true);
			LockOnWidgetFront->SetDrawSize(FVector2D(100.0f, 100.0f));
			LockOnWidgetFront->SetRelativeLocation(FVector(0.0f, 0.0f, 200.1f));
			LockOnWidgetFront->SetRelativeRotation(FRotator(0.0f, 90.f, 0.0f));
			LockOnWidgetFront->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			LockOnWidgetFront->SetVisibility(false);

			if (LockOnWidgetClass)
			{
				UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetWorld(), LockOnWidgetClass);
				LockOnWidgetFront->SetWidget(CreatedWidget);
			}

			LockOnWidgetBack = NewObject<UWidgetComponent>(AICharacter);
			LockOnWidgetBack->SetupAttachment(AICharacter->GetMesh());
			LockOnWidgetBack->RegisterComponent();
			
			LockOnWidgetBack->SetWidgetSpace(EWidgetSpace::World);
			LockOnWidgetBack->SetDrawAtDesiredSize(true);
			LockOnWidgetBack->SetDrawSize(FVector2D(100.0f, 100.0f));
			LockOnWidgetBack->SetRelativeLocation(FVector(0.0f, 0.0f, 199.f));
			LockOnWidgetBack->SetRelativeRotation(FRotator(0.0f, 270.f, 0.0f));
			LockOnWidgetBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			LockOnWidgetBack->SetVisibility(false);

			if (LockOnWidgetClass)
			{
				UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetWorld(), LockOnWidgetClass);
				LockOnWidgetBack->SetWidget(CreatedWidget);
			}
		}
	}
}

void AMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonsterAIController::ToggleLockOnWidget(bool bIsLockOnWidget)
{
	LockOnWidgetFront->SetVisibility(bIsLockOnWidget);
	LockOnWidgetBack->SetVisibility(bIsLockOnWidget);
}

// 개별 액터의 감지/미감지 상태 기반 행동(추격 시작/중단 등)을 할 때
void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (ASLPlayerCharacterBase* Player = Cast<ASLPlayerCharacterBase>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			BlackboardComponent->SetValueAsObject(BlackboardKeys::TargetActor, Actor);
			if (AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn()))
			{
				MyChar->SetChasing(true);
				MyChar->SetPrimaryState(TAG_AI_AbleToAttack);
			}
		}
		else
		{
			BlackboardComponent->ClearValue(BlackboardKeys::TargetActor);
			if (AMonsterAICharacter* MyChar = Cast<AMonsterAICharacter>(GetPawn()))
			{
				MyChar->SetChasing(false);
				MyChar->SetPrimaryState(TAG_AI_Idle);
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
