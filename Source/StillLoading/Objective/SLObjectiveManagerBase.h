// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLObjectiveManagerBase.generated.h"

class USLObjectiveHandlerBase;
class ASLInGameHUD;

UCLASS()
class STILLLOADING_API ASLObjectiveManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASLObjectiveManagerBase();

protected:
	//UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveManager")
	//void ActivateInGameUI();
	//virtual void ActivateInGameUI_Implementation() {};

	//UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveManager")
	//void DeactivateInGameUI();
	//virtual void DeactivateInGameUI_Implementation() {};

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USLObjectiveHandlerBase> ObjectiveHandler = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Objective", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASLInGameHUD> HUD = nullptr;
};
