#include "SLPlayerCharacterBase.h"

#include "BattleComponent/BattleComponent.h"
#include "Buffer/InputBufferComponent.h"
#include "Components/CapsuleComponent.h"
#include "DynamicIMCComponent/SLDynamicIMCComponent.h"

ASLPlayerCharacterBase::ASLPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	if (UInputBufferComponent* BufferComp = FindComponentByClass<UInputBufferComponent>())
	{
		if (UDynamicIMCComponent* IMCComp = FindComponentByClass<UDynamicIMCComponent>())
		{
			IMCComp->OnActionStarted.AddDynamic(BufferComp, &UInputBufferComponent::OnIMCActionStarted);
		}
	}
}

void ASLPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void ASLPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ASLPlayerCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UBattleComponent* BattleComp = FindComponentByClass<UBattleComponent>())
	{
		ISLBattleInterface::Execute_ReceiveBattleDamage(BattleComp, ActualDamage);
	}

	return ActualDamage;
}

