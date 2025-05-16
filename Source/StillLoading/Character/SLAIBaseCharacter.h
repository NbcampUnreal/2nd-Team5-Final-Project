// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLAIBaseCharacter.generated.h"

class UBoxComponent;
class UBlackboardComponent;
class ASLBaseAIController;
class UAnimMontage; 

// --- Enums ---
// AI의 전반적인 상태 (Idle, Moving, Combat, Dead)
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Patrol      UMETA(DisplayName = "Patrol"),
	Suspicious  UMETA(DisplayName = "Suspicious"),
	Chasing     UMETA(DisplayName = "Chasing"),
	Waiting     UMETA(DisplayName = "Waiting"),
	Moving      UMETA(DisplayName = "Moving"),
	Combat		UMETA(DisplayName = "Combat"),
	Dead        UMETA(DisplayName = "Dead")
};

// 피격 방향
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

// AI의 전투 페이즈 
UENUM(BlueprintType)
enum class ECombatPhase : uint8
{
	Phase_None      UMETA(DisplayName = "None / Not In Combat"),
	Phase_One       UMETA(DisplayName = "Phase 1"),
	Phase_Two       UMETA(DisplayName = "Phase 2"),
	Phase_Three     UMETA(DisplayName = "Phase 3"),
	Phase_Final     UMETA(DisplayName = "Final Phase")
};

UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	// --- Constructor ---
	ASLAIBaseCharacter();
	virtual void BeginPlay() override;

	// --- Overrides ---
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// --- Getters (Collision) ---
	// 손 콜리전 컴포넌트 접근자
	FORCEINLINE UBoxComponent* GetLeftHandCollisionBox() const { return LeftHandCollisionBox; }
	FORCEINLINE UBoxComponent* GetRightHandCollisionBox() const { return RightHandCollisionBox; }
	
	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	// --- AI References ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<ASLBaseAIController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAnimInstance> AnimInstancePtr;
	
	// --- Attributes ---    // 이거 베이스 캐릭터로 옮길지도?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth; // 최대 체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float CurrentHealth; // 현재 체력

	// --- State Flags ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsHitReaction; // 피격 반응을 할건지 여부

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsDead; // 사망 상태인지 여부

	// --- Animation Assets ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montages")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;
	
	// --- Collision Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName LeftHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName RightHandCollisionBoxAttachBoneName;
};