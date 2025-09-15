// Fill out your copyright notice in the Description page of Project Settings.

#include "SLInteractableObjectAnimation.h"

#include "AI/AIInterface/SLHideableInterface.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimMontage.h"
#include "StillLoading/Character/SLPlayerCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

ASLInteractableObjectAnimation::ASLInteractableObjectAnimation()
{
	PrimaryActorTick.bCanEverTick = false;
	
	EnterTransformArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("EnterTransformArrow"));
	EnterTransformArrow->SetupAttachment(StaticMeshComp);
	EnterTransformArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
	EnterMontage = nullptr;
	ExitMontage = nullptr;
	bIsPlayerHiding = false;
	CachedCharacter = nullptr;
}

void ASLInteractableObjectAnimation::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);
	
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not valid"));
		return;
	}

	ASLPlayerCharacterBase* MutableCharacter = const_cast<ASLPlayerCharacterBase*>(InCharacter);
	
	if (!CanInteractWithCurrentSection(MutableCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot interact - not in Idle section"));
		return;
	}
	
	CachedCharacter = MutableCharacter;
	
	if (!MutableCharacter->GetClass()->ImplementsInterface(USLHideableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character does not implement ISLHideableInterface"));
		return;
	}
	
	if (!bIsPlayerHiding)
	{
		ExecuteEnterAnimation(MutableCharacter);
		ISLHideableInterface::Execute_SetHidingState(MutableCharacter, true);
		bIsPlayerHiding = true;
	}
	else
	{
		ExecuteExitAnimation(MutableCharacter);
		ISLHideableInterface::Execute_SetHidingState(MutableCharacter, false);
		bIsPlayerHiding = false;
	}
}

bool ASLInteractableObjectAnimation::CanInteractWithCurrentSection(ASLPlayerCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter))
		return false;

	USkeletalMeshComponent* MeshComp = InCharacter->GetMesh();
	if (!IsValid(MeshComp))
		return true; 

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!IsValid(AnimInstance))
		return true;

	if (IsValid(EnterMontage) && AnimInstance->Montage_IsPlaying(EnterMontage))
	{
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(EnterMontage);
		UE_LOG(LogTemp, Log, TEXT("Current Enter Section: %s"), *CurrentSection.ToString());
		
		return CurrentSection == FName("Idle");
	}
	
	if (IsValid(ExitMontage) && AnimInstance->Montage_IsPlaying(ExitMontage))
	{
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(ExitMontage);
		UE_LOG(LogTemp, Log, TEXT("Current Exit Section: %s"), *CurrentSection.ToString());
		
		return CurrentSection == FName("Idle");
	}
	
	return true;
}

void ASLInteractableObjectAnimation::ExecuteEnterAnimation(ASLPlayerCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character or EnterTransformArrow is not valid"));
		return;
	}
	
	FVector TargetLocation = EnterTransformArrow->GetComponentLocation();
	FRotator TargetRotation = EnterTransformArrow->GetComponentRotation();
	
	InCharacter->SetActorLocation(TargetLocation);
	InCharacter->SetActorRotation(TargetRotation);

	OnPlayerEnterHiding(InCharacter);

	InCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	
	if (IsValid(EnterMontage))
	{
		USkeletalMeshComponent* MeshComp = InCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->OnMontageEnded.AddDynamic(this, &ASLInteractableObjectAnimation::OnEnterMontageEnded);
				AnimInstance->Montage_Play(EnterMontage);
				UE_LOG(LogTemp, Log, TEXT("Started Enter Montage from Default section"));
			}
		}
	}
}

void ASLInteractableObjectAnimation::ExecuteExitAnimation(ASLPlayerCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is not valid"));
		return;
	}

	OnPlayerExitHiding(InCharacter);
	
	if (IsValid(ExitMontage))
	{
		USkeletalMeshComponent* MeshComp = InCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				if (IsValid(EnterMontage) && AnimInstance->Montage_IsPlaying(EnterMontage))
				{
					AnimInstance->Montage_Stop(0.2f, EnterMontage);
				}
				
				AnimInstance->OnMontageEnded.AddDynamic(this, &ASLInteractableObjectAnimation::OnExitMontageEnded);
				AnimInstance->Montage_Play(ExitMontage);
				UE_LOG(LogTemp, Log, TEXT("Started Exit Montage"));
			}
		}
	}
	else
	{
		InCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		if (InCharacter->StimuliSource)
		{
			InCharacter->StimuliSource->SetActive(true);
		}
	}
}

void ASLInteractableObjectAnimation::OnEnterMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsValid(CachedCharacter))
	{
		return;
	}

	if (Montage == EnterMontage)
	{

		USkeletalMeshComponent* MeshComp = CachedCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->OnMontageEnded.RemoveDynamic(this, &ASLInteractableObjectAnimation::OnEnterMontageEnded);
			}
		}
	}
}

void ASLInteractableObjectAnimation::OnExitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsValid(CachedCharacter))
	{
		return;
	}

	if (Montage == ExitMontage)
	{
		CachedCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		if (CachedCharacter->StimuliSource)
		{
			CachedCharacter->StimuliSource->SetActive(true);
		}

		USkeletalMeshComponent* MeshComp = CachedCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->OnMontageEnded.RemoveDynamic(this, &ASLInteractableObjectAnimation::OnExitMontageEnded);
			}
		}
	}
}