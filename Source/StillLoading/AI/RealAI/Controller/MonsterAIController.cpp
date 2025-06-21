#include "MonsterAIController.h"

#include "Character/SLPlayerCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionComponent.h"

struct FStateTreeInstanceData;

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(1));

	if (InPawn)
	{
		//CheckPerception(InPawn);
		
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

ETeamAttitude::Type AMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	const FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
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
