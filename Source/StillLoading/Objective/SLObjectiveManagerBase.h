// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLObjectiveManagerBase.generated.h"

class USLObjectiveHandlerBase;
class ASLInGameHUD;
class ASLBattlePlayerState;

UCLASS()
class STILLLOADING_API ASLObjectiveManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASLObjectiveManagerBase();

protected:
	UFUNCTION(BlueprintCallable)
	void ResetCurrentObjective();

	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void ResetCurrentGame();

	UFUNCTION(BlueprintCallable)
	void ActivatePlayerHpUI();

	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void PostPlayerDied();

	UFUNCTION()
	void CheckPlayerDied(float MaxHp, float CurrentHp);

	virtual void BeginPlay() override;

	void GetPlayerState();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USLObjectiveHandlerBase> ObjectiveHandler = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Objective", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASLInGameHUD> HUD = nullptr;

	UPROPERTY()
	TObjectPtr<ASLBattlePlayerState> PlayerState = nullptr;
};