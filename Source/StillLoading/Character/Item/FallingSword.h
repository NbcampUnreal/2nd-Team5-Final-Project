#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallingSword.generated.h"

class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class STILLLOADING_API AFallingSword : public AActor
{
	GENERATED_BODY()

public:
	AFallingSword();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void TriggerImpact();

	// 일정 간격으로 피해를 주기 위한 스윕을 실행하는 함수
	UFUNCTION()
	void ApplySweepDamage();

public:
	// 칼의 외형을 표시할 스태틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SwordMesh;

	// 칼의 낙하 움직임을 처리할 프로젝타일 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// --- 스킬 데이터 (블루프린트에서 수정 가능) ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	float DamagePerTick = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	float DamageInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	float DamageRadius = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	float MaxLifetime = 10.0f;

	// 낙하 시 나타날 궤적 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> TrailEffect;
    
	// 지면에 충돌했을 때 나타날 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	// 지면에 충돌했을 때 재생될 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<USoundBase> ImpactSound;

	// 시작 했을 때 재생될 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<USoundBase> BeginSound;

private:
	// 피해를 주기 위한 타이머 핸들
	FTimerHandle DamageTimerHandle;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	bool bImpactTriggered = false;
};
