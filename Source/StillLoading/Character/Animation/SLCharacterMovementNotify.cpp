#include "SLCharacterMovementNotify.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/Item/FallingSword.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "Character/Skill/SLSkillComponent.h"
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
		/*
		if (UMovementHandlerComponent* MoveComp = Character->FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->StartKnockback(KnockBackSpeed, KnockBackDuration);
		}
		*/

		const FVector ForwardDir = Character->GetActorForwardVector();
		const FVector BackwardDir = -ForwardDir;

		FVector LaunchDirection = BackwardDir + (FVector::UpVector * 0.1);
		LaunchDirection.Normalize();

		LaunchVector = LaunchDirection * LaunchPower;
           
		Character->LaunchCharacter(LaunchVector, true, true);
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

	case ECharacterMovementAction::CMA_EmpowerSword:
		Character->OnEmpoweredStateChanged(true);
		break;

	case ECharacterMovementAction::CMA_DestroySword:
		if (Character->Sword)
		{
			Character->Sword->Destroy();
		}
		break;

	case ECharacterMovementAction::CMA_SpawnDagger:
		Character->SpawnObject(false);
		break;

	case ECharacterMovementAction::CMA_SpawnSword:
		if (USLSkillComponent* SkillComp = Character->FindComponentByClass<USLSkillComponent>())
		{
			SkillComp->ActiveSpawnSkill(Character);
		}
		break;

	default: break;
	}
}
