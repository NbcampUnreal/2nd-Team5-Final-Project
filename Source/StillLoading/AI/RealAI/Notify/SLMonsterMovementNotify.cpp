#include "SLMonsterMovementNotify.h"

#include "AI/RealAI/MonsterAICharacter.h"

void USLMonsterMovementNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AMonsterAICharacter* Character = Cast<AMonsterAICharacter>(MeshComp->GetOwner());
	if (!Character) return;

	FVector LaunchVector = FVector::ZeroVector;

	switch (MovementAction)
	{
	case ECharacterMovementAction::CMA_LaunchUp:
		LaunchVector = FVector::UpVector * LaunchPower;
		Character->LaunchCharacter(LaunchVector, true, false);
		break;

	case ECharacterMovementAction::CMA_LaunchBack:
		break;

	default:
		break;
	}
}
