#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLBaseCharacter.h"
#include "SLCharacter.generated.h"

UCLASS()
class STILLLOADING_API ASLCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Movement")
	float LastLandTime = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, const FName SocketName) const;
	
	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;

	// Debug용 함수
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintPrimaryStateTags() const;

public:
	// 상태 태그 컨테이너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer PrimaryStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer SecondaryStateTags;

	// 상태 태그 추가/제거 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void AddSecondaryState(FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void RemoveSecondaryState(FGameplayTag StateToRemove);

	// 그룹 기반 상태 전환
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetMovementState(FGameplayTag NewMovementState);

	// 상태 검사 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInPrimaryState(FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool HasSecondaryState(FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInMovementState() const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInAttackState() const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInDefenseState() const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInHitReactionState() const;
};
