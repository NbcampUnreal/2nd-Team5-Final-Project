// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "Components/ArrowComponent.h"
#include "SLInteractableObjectTeleport.generated.h"

UCLASS()
class STILLLOADING_API ASLInteractableObjectTeleport : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLInteractableObjectTeleport();

protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// 타겟 위치 및 방향을 시각적으로 표시하는 화살표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> TargetArrow;

	// 목표 트랜스폼 (자동으로 Arrow Component에서 가져옴)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Teleport")
	FTransform TargetTransform;
};