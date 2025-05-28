// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLItem.h"
#include "Character/Interface/OverlapToggleable.h"
#include "SLDefaultStaff.generated.h"

class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLDefaultStaff : public ASLItem, public IOverlapToggleable
{
	GENERATED_BODY()

public:
	ASLDefaultStaff();

	virtual void EnableOverlap() override;
	virtual void DisableOverlap() override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;
};
