// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLInteractableObjectBase.h"
#include "Object/SLInteractableObject.h"
#include "SLInteractableCharacter.generated.h"

class USLTalkHandlerBase;
class USLUISubsystem;

UCLASS()
class STILLLOADING_API ASLInteractableCharacter : public ASLInteractableObject
{
	GENERATED_BODY()

public:
	ASLInteractableCharacter();
	
protected:
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
};
