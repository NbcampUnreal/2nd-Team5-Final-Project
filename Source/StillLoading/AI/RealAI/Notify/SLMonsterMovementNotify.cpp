#include "SLMonsterMovementNotify.h"

#include "AI/RealAI/MonsterAICharacter.h"
#include "Character/SLPlayerCharacter.h"

void USLMonsterMovementNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AMonsterAICharacter* Character = Cast<AMonsterAICharacter>(MeshComp->GetOwner());
	if (!Character) return;

	FVector LaunchVelocity = FVector::ZeroVector;

	switch (MovementAction)
	{
	case ECharacterMovementAction::CMA_LaunchUp:
		LaunchVelocity = FVector::UpVector * LaunchPower;
		Character->LaunchCharacter(LaunchVelocity, true, false);
		break;

	case ECharacterMovementAction::CMA_LaunchBack:
		const FVector ForwardDir = Character->GetActorForwardVector();
		const FVector BackwardDir = -ForwardDir;

		FVector LaunchDirection = BackwardDir + (FVector::UpVector * 0.1);
		LaunchDirection.Normalize();

		LaunchVelocity = LaunchDirection * LaunchPower;
           
		Character->LaunchCharacter(LaunchVelocity, true, true);
		break;

	default:
		break;
	}
}
