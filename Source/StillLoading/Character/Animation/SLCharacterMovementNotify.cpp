#include "SLCharacterMovementNotify.h"
#include "Character/SLPlayerCharacter.h"


void USLCharacterMovementNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	ASLPlayerCharacter* Character = Cast<ASLPlayerCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	switch (MovementAction)
	{
	case ECharacterMovementAction::CMA_Launch:
		Character->LaunchCharacter(LaunchVector, true, true);
		UE_LOG(LogTemp, Log, TEXT("Launch: %s"), *LaunchVector.ToString());
		break;

	default:
		break;
	}
}
