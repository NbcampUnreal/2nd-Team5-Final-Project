// Fill out your copyright notice in the Description page of Project Settings.

#include "SLInteractableObjectTeleport.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Components/ArrowComponent.h"

ASLInteractableObjectTeleport::ASLInteractableObjectTeleport()
{
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;

	// 타겟 화살표 컴포넌트 생성
	TargetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("TargetArrow"));
	TargetArrow->SetupAttachment(StaticMeshComp);
	TargetArrow->SetRelativeLocation(FVector(300.0f, 0.0f, 0.0f));
	TargetArrow->SetArrowColor(FLinearColor::Red);
}

void ASLInteractableObjectTeleport::BeginPlay()
{
	Super::BeginPlay();

	// Arrow Component의 월드 트랜스폼을 타겟으로 설정
	if (TargetArrow)
	{
		TargetTransform = TargetArrow->GetComponentTransform();
	}
}

void ASLInteractableObjectTeleport::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);

	// 캐릭터를 타겟 위치로 텔레포트
	if (InCharacter)
	{
		const_cast<ASLPlayerCharacterBase*>(InCharacter)->SetActorTransform(TargetTransform);
	}
}

#if WITH_EDITOR
void ASLInteractableObjectTeleport::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Arrow Component가 움직일 때마다 TargetTransform 업데이트
	if (TargetArrow)
	{
		TargetTransform = TargetArrow->GetComponentTransform();
	}
}
#endif