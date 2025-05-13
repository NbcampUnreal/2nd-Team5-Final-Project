// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectStatue.generated.h"

UENUM()
enum class ESLStatueType : uint8
{
	EST_None,
	EST_RotatingStatue,    // 회전 석상
	EST_LuminousStatue     // 발광 석상
};

UCLASS()
class STILLLOADING_API ASLReactiveObjectStatue : public ASLBaseReactiveObject
{
	GENERATED_BODY()
	
	
public:
	ASLReactiveObjectStatue();
	virtual void OnReacted(const ASLBaseCharacter* InCharacter, ESLReactiveTriggerType TriggerType) override;
	UFUNCTION()
	void DeactivateStatue();
protected:
	virtual void BeginPlay();

	//UFUNCTION()
	//void LuminousStatue(const ASLBaseCharacter* InCharacter);
private:
	//각 석상별 고유번호
	UPROPERTY(EditAnywhere)
	int8 StatueIndex = 0;

};
