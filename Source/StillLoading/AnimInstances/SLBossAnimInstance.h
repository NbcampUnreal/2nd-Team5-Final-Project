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
	
	// 액터를 생성하는 함수 - 소켓 이름 매개변수 추가
	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* SpawnActorToThrow(TSubclassOf<AActor> ActorClass = nullptr, FName SocketName = NAME_None);
    
	// 액터를 던지는 함수 - 소켓 이름 매개변수 추가
	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* ThrowActorAtTarget(float LaunchSpeed = 1000.f, float ArcParam = 0.5f, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool JumpToTarget(float JumpSpeed = 700.f, float JumpArc = 0.7f, bool bUpdateRotation = true, float RemainingAnimTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FinishJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool ChargeToTarget(float ChargeSpeed = 1000.f, bool bUpdateRotation = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FinishCharge();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	EBossAttackPattern GetBossAttackPattern();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> ActorToThrow;
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossAttackPattern BossAttackPattern;

	FTimerHandle JumpTimerHandle;
	
	// 원래 충돌 설정 저장
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionType;
};
