// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "Character/SLPlayerCharacterBase.h"
#include "SLPlayerRunnerCharacter.generated.h"

class UDynamicIMCComponent;
class UBoxComponent;
class USLRunnerAnimInstance;
enum class EInputActionType : uint8;
struct FInputActionValue;

UCLASS()
class STILLLOADING_API ASLPlayerRunnerCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ASLPlayerRunnerCharacter();

	virtual void BeginPlay() override;

protected:
	// 입력 디스패처 콜백
	UFUNCTION()
	void OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue);

	// 허들 충돌
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 데미지/사망
	void ApplyDamage();
	void OnDie();

protected:
	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDynamicIMCComponent> DynamicIMCComponent;

	// --- State / Health ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|State")
	EHurdleState CurrentState = EHurdleState::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Health")
	int32 MaxHealth = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Runner|Health")
	int32 CurrentHealth = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Health")
	float RecoveryTime = 2.0f;

	// 액션 유지 타이머(0.3~0.5s)
	FTimerHandle StateTimerHandle;

	// 체력 회복 타이머
	FTimerHandle RecoveryTimerHandle;

	// 연속 피격 방지(i-Frame)
	bool bInvincible = false;
	FTimerHandle IFrameTimerHandle;
	UPROPERTY(EditAnywhere, Category="Runner|Combat")
	float IFrameDuration = 0.2f;

private:
	USLRunnerAnimInstance* GetRunnerAnim() const;
	void PushToAnim(ERunnerAction Action) const;
	void PlayMatched(EHurdleState State) const;
};