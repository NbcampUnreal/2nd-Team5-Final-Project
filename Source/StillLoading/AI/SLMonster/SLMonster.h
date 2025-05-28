// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLMonster.generated.h"

UCLASS()
class STILLLOADING_API ASLMonster : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLMonster();

	UFUNCTION(BlueprintCallable, Category = "RVO")
	void SetRVOAvoidanceEnabled(bool bEnable);
	UFUNCTION(BlueprintCallable)
	bool GetIsInCombat() const;
	UFUNCTION()
	void SetIsInCombat(bool bInCombat);
	UFUNCTION(BlueprintCallable)
	bool IsTargetInSight() const;

	virtual void Tick(float DeltaTime) override;

	void Attack();

	void SetPatrolPoints(const TArray<FVector>& Points);

    UPROPERTY()
    TObjectPtr<AActor> TargetActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="AI")
	float AttackAngle = 80.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="AI")
	class USphereComponent* AttackSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AttackCheckSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bIsCombat = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 100.0f;
	UPROPERTY(EditAnywhere, Category = "AI")

	float AttackCooldown = 0.7f;

	float LastAttackTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceRadius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceWeight = 0.5f;
	UPROPERTY(BlueprintReadOnly, Category ="Combat")
	int32 RandomAttackInt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RandCoolTime;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	bool IsCloseToTargetPoint(float AcceptableDistance = 200.f);

	void MoveToNextPatrolPoint();

	FVector CurrentTargetLocation;

	TArray<FVector> PatrolPoints;

	int32 CurrentPatrolIndex = 0;

	TObjectPtr<UBlackboardComponent> BlackboardComp;
};
