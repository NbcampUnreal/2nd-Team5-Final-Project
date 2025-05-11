// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLReactiveObjectStatue.h"
#include "SLReactiveObjectLuminousStatue.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLReactiveObjectLuminousStatue : public ASLReactiveObjectStatue
{
	GENERATED_BODY()
	
protected:
	virtual void OnReacted(const ASLBaseCharacter* InCharacter) override;

private:

};
