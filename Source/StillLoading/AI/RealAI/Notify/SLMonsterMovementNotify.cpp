#include "SLMonsterMovementNotify.h"

#include "AI/RealAI/SLMonsterAICharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void USLMonsterMovementNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	ASLMonsterAICharacter* Character = Cast<ASLMonsterAICharacter>(MeshComp->GetOwner());
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

	case ECharacterMovementAction::CMA_AISpear:
		Character->SpawnSpear();
		break;

	case ECharacterMovementAction::CMA_AISpawnArrow:
		Character->SpawnArrow();
		break;

	case ECharacterMovementAction::CMA_AIWeaponVisible:
		Character->ToggleWeaponState(bIsVisible);
		break;

	case ECharacterMovementAction::CMA_RotateFaceToTarget:
		break;

	case ECharacterMovementAction::CMA_EndRotate:
		break;

	case ECharacterMovementAction::CMA_CorrectLocation:
		Character->CorrectActorLocationPostAttack();
		break;

	default:
		break;
	}
}
