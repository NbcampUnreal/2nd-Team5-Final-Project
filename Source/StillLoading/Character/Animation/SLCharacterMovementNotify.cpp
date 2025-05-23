#include "SLCharacterMovementNotify.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

void USLCharacterMovementNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	ASLPlayerCharacter* Character = Cast<ASLPlayerCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	FVector LaunchVector = FVector::ZeroVector;

	switch (MovementAction)
	{
	case ECharacterMovementAction::CMA_LaunchUp:
		LaunchVector = FVector::UpVector * LaunchPower;
		Character->LaunchCharacter(LaunchVector, true, false);
		break;

	case ECharacterMovementAction::CMA_LaunchBack:
		if (UMovementHandlerComponent* MoveComp = Character->FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->StartKnockback(KnockBackSpeed, KnockBackDuration);
		}
		break;

	default:
		break;
	}
}
