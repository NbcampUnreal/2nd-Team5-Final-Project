#include "SLPlayerCharacterBase.h"

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

