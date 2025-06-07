#include "SLCharacterMovementNotify.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	case ECharacterMovementAction::CMA_LaunchOff:
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		break;

	case ECharacterMovementAction::CMA_LaunchActionSword:
		if (UMovementHandlerComponent* MoveComp = Character->FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->ToggleCameraZoom(false);
		}
		
		Character->BeginBlast(EItemType::IT_Sword);
		break;

	case ECharacterMovementAction::CMA_LaunchActionShield:
		if (UMovementHandlerComponent* MoveComp = Character->FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->ToggleCameraZoom(false);
		}
		
		Character->BeginBlast(EItemType::IT_Shield);
		break;

	default: break;
	}
}
