// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLInteractableObject.generated.h"

class UBoxComponent;
class USLBaseTextPrintWidget;
class USLUISubsystem;
class USLTalkHandlerBase;
class USoundBase;

UCLASS()
class STILLLOADING_API ASLInteractableObject : public ASLInteractableObjectBase
{
	GENERATED_BODY()

public:
	ASLInteractableObject();
	
	UFUNCTION(BlueprintCallable, Category = "InteractableObject")
	void SetCurrentTalkHandler(USLTalkHandlerBase* TalkHandler);
	UFUNCTION(BlueprintCallable, Category = "InteractableObject")
	USLTalkHandlerBase* GetCurrentTalkHandler();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;
	
	UFUNCTION()
	virtual void StartTalk();
	
	UFUNCTION()
	void OnCurrentTalkEnd();
	UFUNCTION()
	void OnCurrentChoiceEnd(bool bResult);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject")
	FName TargetName;
	
	UPROPERTY(VisibleInstanceOnly, Category = "InteractableObject", DisplayName = "현재 대화 핸들러")
	TWeakObjectPtr<USLTalkHandlerBase> CurrentTalkHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject")
	TObjectPtr<UBoxComponent> InteractionCollision;
	
	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;
	
	FTimerHandle TalkDelayTimer;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject", DisplayName="기본 대화 핸들러" ,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USLTalkHandlerBase> DefaultTalkHandler;
	
};
