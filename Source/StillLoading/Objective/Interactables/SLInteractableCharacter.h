// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLInteractableObjectBase.h"
#include "GameFramework/Actor.h"
#include "SLInteractableCharacter.generated.h"

class USLUISubsystem;

UCLASS()
class STILLLOADING_API ASLInteractableCharacter : public AActor, public ISLInteractableObjectBase
{
	GENERATED_BODY()

public:
	ASLInteractableCharacter();
	virtual void Interaction() override;
	
protected:
	virtual void BeginPlay() override;

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
