// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLInteractableCharacter.generated.h"

class USLTalkHandlerBase;
class USLUISubsystem;

UCLASS()
class STILLLOADING_API ASLInteractableCharacter : public ASLBaseReactiveObject
{
	GENERATED_BODY()

public:
	ASLInteractableCharacter();
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

private:
	UFUNCTION()
	void OnCurrentTalkEnd();
	
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	FName TargetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USLTalkHandlerBase> BaseTalkHandler;

	TWeakObjectPtr<USLTalkHandlerBase> CurrentTalkHandler;
};
