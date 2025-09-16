#include "SLAIPushComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

USLAIPushComponent::USLAIPushComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OverlapDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapDetectionSphere"));
	OverlapDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapDetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapDetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OverlapDetectionSphere->SetSphereRadius(100.0f);
}

void USLAIPushComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* MyCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (UCapsuleComponent* Capsule = MyCharacter->GetCapsuleComponent())
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		}
	}
}
