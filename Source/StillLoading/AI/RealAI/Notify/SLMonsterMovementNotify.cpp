#include "SLMonsterMovementNotify.h"

#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmManager.h"
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
		{
			ASwarmAgent* Agent = Cast<ASwarmAgent>(MeshComp->GetOwner());
			if (!Agent) return;

			AActor* Target = Agent->MySwarmManager->CurrentSquadTarget;
			if (!Target) return;

			FVector StartLocation = Agent->GetActorLocation();
			FVector TargetLocation = Target->GetActorLocation();
			FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();

			Direction.Z = 0.0f;

			FRotator TargetRotation = Direction.Rotation();

			Agent->SetActorRotation(TargetRotation);
			break;
		}

	case ECharacterMovementAction::CMA_EndRotate:
		{
			if (AActor* OwnerActor = MeshComp->GetOwner())
			{
				if (ASwarmAgent* Agent = Cast<ASwarmAgent>(OwnerActor))
				{
					Agent->StopSpinning();
				}
			}
		}
		break;

	default:
		break;
	}
}
