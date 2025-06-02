// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAIProjectile.h"
#include "SLHomingProjectile.generated.h"

UCLASS()
class STILLLOADING_API ASLHomingProjectile : public ASLAIProjectile
{
	GENERATED_BODY()

public:
	ASLHomingProjectile();

	// 추적 대상 설정
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetHomingTarget(AActor* Target);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 추적 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingAccelerationMagnitude;

	// 추적 시작 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingDelay;

private:
	// 추적 시작
	void StartHoming();
	
	// 추적 대상
	UPROPERTY()
	TWeakObjectPtr<AActor> HomingTarget;

	// 추적 시작 여부
	bool bIsHoming;
    
	// 타이머 핸들
	FTimerHandle HomingDelayTimer;
};
