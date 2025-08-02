// Fill out your copyright notice in the Description page of Project Settings.

#include "SLInteractableObjectAnimation.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimMontage.h"
#include "StillLoading/Character/SLPlayerCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

ASLInteractableObjectAnimation::ASLInteractableObjectAnimation()
{
	PrimaryActorTick.bCanEverTick = false;
	
	EnterTransformArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("EnterTransformArrow"));
	EnterTransformArrow->SetupAttachment(StaticMeshComp);
	EnterTransformArrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	// 기본값 설정
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
	EnterMontage = nullptr;
	ExitMontage = nullptr;
	bIsPlayerHiding = false;
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

	// 블루프린트 이벤트 호출
	OnPlayerEnterHiding(InCharacter);
	
	// 들어가는 몽타주 실행
	if (IsValid(EnterMontage))
	{
		USkeletalMeshComponent* MeshComp = InCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				
				float MontageLength = AnimInstance->Montage_Play(EnterMontage);
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

	// 블루프린트 이벤트 호출
	OnPlayerExitHiding(InCharacter);
	
	if (IsValid(ExitMontage))
	{
		USkeletalMeshComponent* MeshComp = InCharacter->GetMesh();
		if (IsValid(MeshComp))
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				float MontageLength = AnimInstance->Montage_Play(ExitMontage);
			}
		}
	}
}