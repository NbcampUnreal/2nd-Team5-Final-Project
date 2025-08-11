// Fill out your copyright notice in the Description page of Project Settings.

#include "SLInteractableObjectAnimation.h"
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
	CachedCharacter = MutableCharacter;
	
	if (!bIsPlayerHiding)
	{
		ExecuteEnterAnimation(MutableCharacter);
		bIsPlayerHiding = true;
	}
	else
	{
		ExecuteExitAnimation(MutableCharacter);
		bIsPlayerHiding = false;
	}
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
			}
		}
	}
}

void ASLInteractableObjectAnimation::ExecuteExitAnimation(ASLPlayerCharacterBase* InCharacter)
{
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character or ExitTransformArrow is not valid"));
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
				AnimInstance->OnMontageEnded.AddDynamic(this, &ASLInteractableObjectAnimation::OnExitMontageEnded);
				AnimInstance->Montage_Play(ExitMontage);
			}
		}
	}
	else
	{
		// 몽타주가 없으면 즉시 콜리전 복구
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
		// 들어가는 애니메이션이 끝나면 AI 감지 비활성화
		if (CachedCharacter->StimuliSource)
		{
			CachedCharacter->StimuliSource->SetActive(false);
		}

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
		// 나가는 애니메이션이 끝나면 콜리전 복구
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