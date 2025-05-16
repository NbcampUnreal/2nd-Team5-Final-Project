#include "AnimationMontageComponent.h"

#include "Character/DataAsset/MontageDataAsset.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimationMontageComponent::UAnimationMontageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAnimationMontageComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!OwnerCharacter) return;

	GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UAnimationMontageComponent::OnMontageBlendingOut);
	}
}

void UAnimationMontageComponent::GetAnimInstance()
{
	AnimInstance = OwnerCharacter ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
}

void UAnimationMontageComponent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage || !GetOwner()) return;
	
	if (Montage == (MontageData ? MontageData->AttackMontage : nullptr))
	{
		UE_LOG(LogTemp, Log, TEXT("Attack Montage Ended"));
	}
	else if (Montage == (MontageData ? MontageData->DodgeMontage : nullptr))
	{
		UE_LOG(LogTemp, Log, TEXT("Dodge Montage Ended"));
	}
}

void UAnimationMontageComponent::PlayMontage(UAnimMontage* Montage, FName Section)
{
	//OwnerCharacter->GetCharacterMovement()->DisableMovement();
	
	if (Montage)
	{
		if (AnimInstance)
		{
			if (Section != NAME_None)
			{
				AnimInstance->Montage_Play(Montage, 1.5f);
				AnimInstance->Montage_JumpToSection(Section, Montage);
			}
			else
			{
				AnimInstance->Montage_Play(Montage);
			}
		}
	}
}

void UAnimationMontageComponent::PlayAttackMontage(FName Section)
{
	if (MontageData)
		PlayMontage(MontageData->AttackMontage, Section);
}

void UAnimationMontageComponent::PlayDodgeMontage(FName Section)
{
	if (MontageData)
		PlayMontage(MontageData->DodgeMontage, Section);
}

void UAnimationMontageComponent::PlayHitReactMontage()
{
	if (MontageData)
		PlayMontage(MontageData->HitReactMontage, NAME_None);
}

void UAnimationMontageComponent::PlayDeathMontage()
{
	if (MontageData)
		PlayMontage(MontageData->DeathMontage, NAME_None);
}

void UAnimationMontageComponent::PlayDefenceMontage()
{
	if (MontageData)
		PlayMontage(MontageData->DefenceMontage, NAME_None);
}

void UAnimationMontageComponent::StopDefenceMontage()
{
	if (MontageData)
		StopMontage(MontageData->DefenceMontage, 0.2f);
}

void UAnimationMontageComponent::StopMontage(UAnimMontage* Montage, float BlendOutTime)
{
	if (AnimInstance && Montage && AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Stop(BlendOutTime, Montage);
	}
}

bool UAnimationMontageComponent::IsAttackMontagePlaying() const
{
    return IsMontagePlayingHelper(MontageData ? MontageData->AttackMontage : nullptr);
}

bool UAnimationMontageComponent::IsMontagePlayingHelper(const UAnimMontage* Montage) const
{
    if (AnimInstance && Montage)
    {
        return AnimInstance->Montage_IsPlaying(Montage);
    }
    
    return false;
}