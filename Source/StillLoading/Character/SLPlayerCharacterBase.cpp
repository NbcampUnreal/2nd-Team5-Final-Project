#include "SLPlayerCharacterBase.h"

#include "Buffer/InputBufferComponent.h"
#include "Components/CapsuleComponent.h"
#include "DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ASLPlayerCharacterBase::ASLPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// AIPerception에 등록
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>(UAISense_Sight::StaticClass()));
	StimuliSource->bAutoRegister = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// BattleComponent 에서 사용 하기위한 캡슐 셋팅
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	if (UInputBufferComponent* BufferComp = FindComponentByClass<UInputBufferComponent>())
	{
		if (UDynamicIMCComponent* IMCComp = FindComponentByClass<UDynamicIMCComponent>())
		{
			IMCComp->OnActionStarted.AddDynamic(BufferComp, &UInputBufferComponent::OnIMCActionStarted);
		}
	}
}

FGenericTeamId ASLPlayerCharacterBase::GetGenericTeamId() const
{
	if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

void ASLPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

