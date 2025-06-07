// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "SLCompanionNormalAIController.generated.h"


UCLASS()
class STILLLOADING_API ASLCompanionNormalAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLCompanionNormalAIController();

	// 플레이어 캐릭터 설정
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetPlayerToFollow(ACharacter* PlayerCharacter);

	// 팀 태도 오버라이드
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

private:
	// 플레이어 캐릭터 참조
	UPROPERTY()
	TObjectPtr<ACharacter> PlayerToFollow;

};