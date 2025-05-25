// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "Character/SLBossCharacter.h"
#include "SLBossAnimInstance.generated.h"

UCLASS()
class STILLLOADING_API USLBossAnimInstance : public USLAICharacterAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBossAttackPattern(EBossAttackPattern NewBossAttackPattern);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* SpawnActorToThrow(TSubclassOf<AActor> ActorClass = nullptr, FName SocketName = NAME_None);
    
	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* ThrowActorAtTarget(float LaunchSpeed = 1000.f, float TimeToTarget = 0.5f, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool JumpToTargetPoint(AActor* TargetPointActor, bool bUpdateRotation, float RemainingAnimTime, float OffsetDistance);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FinishJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool ChargeToTarget(float ChargeSpeed = 1000.f, bool bUpdateRotation = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FinishCharge();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void CleanupJumpTimers();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EBossAttackPattern GetBossAttackPattern();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CleanupThrowActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> ActorToThrow;

	FTimerHandle JumpTimerHandle;

	// 원래 충돌 설정 저장
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionType;
private:
	// 착지 체크 함수
	UFUNCTION()
	void CheckForLanding();
	
	UFUNCTION()
	void CompleteLanding();
	
	UFUNCTION()
	void SetupJumpCollision();
	
	UFUNCTION()
	void RestoreJumpCollision();
	
	UFUNCTION()
	void StartLandingCheck();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossAttackPattern BossAttackPattern;

	// 착지 모니터링을 위한 타이머
	FTimerHandle LandingCheckTimer;
    
	// 목표 착지 위치
	FVector TargetLandingLocation;
};
