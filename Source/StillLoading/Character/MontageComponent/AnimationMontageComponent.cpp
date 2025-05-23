#include "AnimationMontageComponent.h"

#include "Character/DataAsset/MontageDataAsset.h"
#include "GameFramework/Character.h"

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
	else if (Montage == (MontageData ? MontageData->SkillMontage : nullptr))
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

void UAnimationMontageComponent::PlaySkillMontage(FName Section)
{
	if (MontageData)
		PlayMontage(MontageData->SkillMontage, Section);
}

void UAnimationMontageComponent::PlayHitMontage(FName Section)
{
	if (MontageData)
		PlayMontage(MontageData->HitMontage, Section);
}

void UAnimationMontageComponent::PlayBlockMontage(FName Section)
{
	if (MontageData)
		PlayMontage(MontageData->BlockMontage, Section);
}

void UAnimationMontageComponent::StopAllMontages(float BlendOutTime)
{
	if (!AnimInstance) return;

	AnimInstance->Montage_Stop(BlendOutTime);
}

void UAnimationMontageComponent::StopActiveMontages(float BlendOutTime)
{
	if (!AnimInstance) return;

	if (IsMontagePlayingHelper(MontageData->AttackMontage))
		StopMontage(MontageData->AttackMontage, BlendOutTime);

	if (IsMontagePlayingHelper(MontageData->SkillMontage))
		StopMontage(MontageData->SkillMontage, BlendOutTime);

	if (IsMontagePlayingHelper(MontageData->AirAttackMontage))
		StopMontage(MontageData->AirAttackMontage, BlendOutTime);

	if (IsMontagePlayingHelper(MontageData->SpecialAttackMontage))
		StopMontage(MontageData->SpecialAttackMontage, BlendOutTime);
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