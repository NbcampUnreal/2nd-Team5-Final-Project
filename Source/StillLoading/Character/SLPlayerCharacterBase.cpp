#include "SLPlayerCharacterBase.h"

#include "BattleComponent/BattleComponent.h"
#include "Buffer/InputBufferComponent.h"
#include "Camera/CameraComponent.h"
#include "CameraManagerComponent/CameraManagerComponent.h"
#include "DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASLPlayerCharacterBase::ASLPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

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

