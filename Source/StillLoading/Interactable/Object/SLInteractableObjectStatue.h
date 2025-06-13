// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLInteractableObjectStatue.generated.h"

UENUM()
enum class ESLStatueType : uint8
{
	EST_None,
	EST_RotatingStatue,    // 회전 석상
	EST_LuminousStatue     // 발광 석상
};

UCLASS()
class STILLLOADING_API ASLInteractableObjectStatue : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
	
public:
	ASLInteractableObjectStatue();
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
	UFUNCTION()
	void DeactivateStatue();
protected:
	virtual void BeginPlay();

	UPROPERTY(EditAnywhere)
	int8 StatueIndex = 0; //각 석상별 고유번호
	//UFUNCTION()
	//void LuminousStatue(const ASLBaseCharacter* InCharacter);


};
