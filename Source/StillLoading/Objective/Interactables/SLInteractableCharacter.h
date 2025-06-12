// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLInteractableCharacter.generated.h"

class USLUISubsystem;

// TODO: Reactibve 어쩌구로 바꾸기

UCLASS()
class STILLLOADING_API ASLInteractableCharacter : public ASLBaseReactiveObject
{
	GENERATED_BODY()

public:
	ASLInteractableCharacter();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

private:
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FName CurrentTargetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	int32 CurrentTalkIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	TArray<FName> CurrentTalkNames;
};
