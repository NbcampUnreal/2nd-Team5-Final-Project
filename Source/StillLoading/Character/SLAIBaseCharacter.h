// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLAIBaseCharacter.generated.h"

class UBoxComponent;
class UBlackboardComponent;
class ASLBaseAIController;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Moving      UMETA(DisplayName = "Moving"),
	Combat		UMETA(DisplayName = "Combat"),
	Dead        UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

UENUM(BlueprintType)
enum class ECombatPhase : uint8
{
	Phase_None      UMETA(DisplayName = "None / Not In Combat"), // 일반 몬스터의 페이즈 
	Phase_One       UMETA(DisplayName = "Phase 1"),
	Phase_Two       UMETA(DisplayName = "Phase 2"),
	Phase_Three     UMETA(DisplayName = "Phase 3"),
	Phase_Final     UMETA(DisplayName = "Final Phase")         
};

UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	None                UMETA(DisplayName = "None"), // 공격 없음 또는 기본 상태
	Attack_01           UMETA(DisplayName = "Attack 01"),
	Attack_02           UMETA(DisplayName = "Attack 02"),
	Attack_03           UMETA(DisplayName = "Attack 03"),
	Attack_04           UMETA(DisplayName = "Attack 04"),
	DashAttack          UMETA(DisplayName = "Dash Attack"),
	FootAttack_Left     UMETA(DisplayName = "Foot Attack - Left"),
	FootAttack_Right    UMETA(DisplayName = "Foot Attack - Right"),
	GroundSlam_01       UMETA(DisplayName = "Ground Slam 01"),
	GroundSlam_02       UMETA(DisplayName = "Ground Slam 02"),
	JumpAttack          UMETA(DisplayName = "Jump Attack"),
	ThrowStone          UMETA(DisplayName = "Throw Stone"),
	Whirlwind           UMETA(DisplayName = "Whirlwind Attack")
};

UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLAIBaseCharacter();
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	FORCEINLINE UBoxComponent* GetLeftHandCollisionBox() const { return LeftHandCollisionBox; }
	FORCEINLINE UBoxComponent* GetRightHandCollisionBox() const { return RightHandCollisionBox; }

	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// 플레이어를 바라보는 상태인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	bool ShouldLookAtPlayer;

	//공격중인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsAttacking;
	
	//맞았을때 히트 애니메이션을 재생할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsHitReaction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool IsDead;

	//넘어짐
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool IsDown;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float CurrentHealth;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montages")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	EBossAttackPattern BossAttackPattern;
private:
	// AI 컨트롤러 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASLBaseAIController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName LeftHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RightHandCollisionBoxAttachBoneName;
;
};
