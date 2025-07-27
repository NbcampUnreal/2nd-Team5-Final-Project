#include "SLAIPushComponent.h"

#include "AI/RealAI/Controller/SLMonsterAIController.h"
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

	if (OverlapDetectionSphere)
	{
		OverlapDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &USLAIPushComponent::OnOwnerBeginOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: OverlapDetectionSphere가 유효하지 않습니다!"), *GetOwner()->GetName());
	}
}

int32 USLAIPushComponent::GetTeamIDFromController(AController* Controller) const
{
	if (Controller)
	{
		if (const ASLMonsterAIController* AIController = Cast<ASLMonsterAIController>(Controller))
		{
			return AIController->GetGenericTeamId();
		}
	}
	return -1;
}

void USLAIPushComponent::OnOwnerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);

		if (!OwnerCharacter || !OtherCharacter) return;

		AController* OwnerController = OwnerCharacter->GetController();
		const int32 OwnerTeamID = GetTeamIDFromController(OwnerController);

		AController* OtherController = OtherCharacter->GetController();
		const int32 OtherTeamID = GetTeamIDFromController(OtherController);

		if (OwnerTeamID != -1 && OtherTeamID != -1 && OwnerTeamID == OtherTeamID)
		{
			FVector PushDirection = GetOwner()->GetActorLocation() - OtherActor->GetActorLocation();
			PushDirection.Z = 0.0f;
			PushDirection.Normalize();

			OtherCharacter->LaunchCharacter(-PushDirection * PushForce, true, false);
		}
	}
}
