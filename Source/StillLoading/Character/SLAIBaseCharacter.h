// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLPlayerCharacterBase.h"
#include "DataAsset/AttackDataAsset.h"
#include "SLAIBaseCharacter.generated.h"

class ASLAIProjectile;
class UBattleComponent;
class UMotionWarpingComponent;
class UBoxComponent;
class UBlackboardComponent;
class ASLBaseAIController;
class UAnimMontage; 

// --- Enums ---
// AI의 전반적인 상태 (Idle, Moving, Combat, Dead)
UENUM(BlueprintType)
enum class EAIState : uint8
{
	EAIS_Idle        UMETA(DisplayName = "Idle"),
	EAIS_Patrol      UMETA(DisplayName = "Patrol"),
	EAIS_Suspicious  UMETA(DisplayName = "Suspicious"),
	EAIS_Chasing     UMETA(DisplayName = "Chasing"),
	EAIS_Waiting     UMETA(DisplayName = "Waiting"),
	EAIS_Moving      UMETA(DisplayName = "Moving"),
	EAIS_Combat		UMETA(DisplayName = "Combat"),
	EAIS_Dead        UMETA(DisplayName = "Dead")
};

// 피격 방향
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	EHD_Front,
	EHD_Back,
	EHD_Left,
	EHD_Right
};

// AI의 전투 페이즈 
UENUM(BlueprintType)
enum class ECombatPhase : uint8
{
	ECP_Phase_None      UMETA(DisplayName = "None / Not In Combat"),
	ECP_Phase_One       UMETA(DisplayName = "Phase 1"),
	ECP_Phase_Two       UMETA(DisplayName = "Phase 2"),
	ECP_Phase_Three     UMETA(DisplayName = "Phase 3"),
	ECP_Phase_Final     UMETA(DisplayName = "Final Phase")
};

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	ETDT_CurrentEquippedWeapon,
	ETDT_LeftHand,
	ETDT_RightHand,
	ETDT_LeftFoot,
	ETDT_RightFoot
};

UENUM(BlueprintType)
enum class EChapter : uint8
{
	EC_None          UMETA(DisplayName = "None"),
	EC_Tutorial      UMETA(DisplayName = "Tutorial"),
	EC_Chapter1      UMETA(DisplayName = "Chapter 1"),
	EC_Chapter2      UMETA(DisplayName = "Chapter 2"),
	EC_Chapter3      UMETA(DisplayName = "Chapter 3"),
	EC_Chapter4      UMETA(DisplayName = "Chapter 4")
};

UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ASLPlayerCharacterBase
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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAttackPower() const { return AttackPower;}

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetIsHitReaction(bool bNewIsHitReaction);

	UFUNCTION(BlueprintCallable)
	void SetAttackPower(float NewAttackPower);

	UFUNCTION(BlueprintCallable)
	void SetCombatPhase(ECombatPhase NewCombatPhase);

	UFUNCTION(BlueprintCallable)
	ECombatPhase GetCombatPhase();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleCollision(EToggleDamageType DamageType, bool bEnableCollision);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleLeftHandCollision(bool bEnableCollision);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleRightHandCollision(bool bEnableCollision);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleLeftFootCollision(bool bEnableCollision);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleRightFootCollision(bool bEnableCollision);
	
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void EquipWeapon(AActor* WeaponActor);

	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable)
	UBattleComponent* GetBattleComponent();

	UFUNCTION(BlueprintCallable)
	void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType);

	UFUNCTION(BlueprintCallable)
	void SetCurrentAttackType(EAttackAnimType NewCurrentAttackType);

	UFUNCTION(BlueprintCallable)
	EAttackAnimType GetCurrentAttackType();

	UFUNCTION(BlueprintCallable, Category = "Chapter")
	EChapter GetChapter() const;

	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	ASLAIProjectile* SpawnProjectileAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass, FVector TargetLocation, FName SocketName = NAME_None, float ProjectileSpeed = 2000.0f, EAttackAnimType AnimType = EAttackAnimType::AAT_Attack_01);

	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	FRotator CalculateProjectileRotation(const FVector& StartLocation, const FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	TArray<ASLAIProjectile*> SpawnProjectileFanAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass, FVector TargetLocation, FName SocketName = NAME_None, int32 ProjectileCount = 5, float FanHalfAngle = 30.0f, float ProjectileSpeed = 2000.0f, EAttackAnimType AnimType = EAttackAnimType::AAT_Attack_01);
protected:
	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	TArray<FVector> GenerateHorizontalFanDirections(const FVector& BaseDirection, int32 Count, float FanHalfAngle) const;

	// --- AI References ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<ASLBaseAIController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAnimInstance> AnimInstancePtr;

	// --- Attributes ---    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth; // 최대 체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float CurrentHealth; // 현재 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float AttackPower;

	// --- State Flags ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsHitReaction; // 피격 반응을 할건지 여부

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsDead; // 사망 상태인지 여부

	// --- Chapter Info ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chapter", meta = (AllowPrivateAccess = "true"))
	EChapter AIChapter;

	// --- Animation Assets ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montages")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;
	
	// --- Collision Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> LeftHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName LeftHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> RightHandCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName RightHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> LeftFootCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName LeftFootCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> RightFootCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision", meta = (AllowPrivateAccess = "true"))
	FName RightFootCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	ECombatPhase CombatPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	TObjectPtr<UPrimitiveComponent> CurrentWeaponCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "combat")
	EAttackAnimType CurrentAttackType;
};


