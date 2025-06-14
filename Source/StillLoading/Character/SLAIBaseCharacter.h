// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectTypes.h"
#include "NiagaraSystem.h"
#include "SLPlayerCharacterBase.h"
#include "DataAsset/AttackDataAsset.h"
#include "SLAIBaseCharacter.generated.h"

class UNiagaraComponent;
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

UENUM(BlueprintType)
enum class EHitReactionMode : uint8
{
	EHRM_Always         UMETA(DisplayName = "Always React"),
	EHRM_Threshold      UMETA(DisplayName = "Damage Threshold"),
	EHRM_Disabled       UMETA(DisplayName = "Disabled")
};



UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// --- Constructor ---
	ASLAIBaseCharacter();
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ASLAIBaseCharacter*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPatternFinished, ASLAIBaseCharacter*);
	
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
	FORCEINLINE bool GetIsDead() const { return IsDead;}

	// Getter 함수들 추가
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsHit() const { return bIsHit; }
    
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDown() const { return bIsDown; }
    
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsStun() const { return bIsStun; }
    
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
    
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EHitDirection GetHitDirection() const { return HitDirection; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetHitDirectionVector() const { return HitDirectionVector; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShouldLookAtPlayer() const { return bShouldLookAtPlayer; }

	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	FORCEINLINE AActor* GetEquippedWeapon() const { return EquippedWeapon; }
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetIsHitReaction(bool bNewIsHitReaction);

	UFUNCTION(BlueprintCallable)
	void SetIsHit(bool bNewIsHit);

	UFUNCTION(BlueprintCallable)
	void SetIsDown(bool bNewIsDown);

	UFUNCTION(BlueprintCallable)
	void SetIsStun(bool bNewIsStun);

	UFUNCTION(BlueprintCallable)
	void SetShouldLookAtPlayer(bool bNewShouldLookAtPlayer);
	
	UFUNCTION(BlueprintCallable)
	void SetIsAttacking(bool bNewIsAttacking);
	
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
	void SetCurrentAttackType(EAttackAnimType NewCurrentAttackType);

	UFUNCTION(BlueprintCallable)
	EAttackAnimType GetCurrentAttackType();

	UFUNCTION(BlueprintCallable, Category = "Chapter")
	EChapter GetChapter() const;

	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	ASLAIProjectile* SpawnProjectileAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass, FVector TargetLocation, FName SocketName = NAME_None, float ProjectileSpeed = 2000.0f, EAttackAnimType AnimType = EAttackAnimType::AAT_Attack_01, bool bHorizontalOnly = false );

	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	FRotator CalculateProjectileRotation(const FVector& StartLocation, const FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	ASLAIProjectile* SpawnProjectileForLaunch(TSubclassOf<ASLAIProjectile> ProjectileClass, FVector TargetLocation, FName SocketName, float ProjectileSpeed, EAttackAnimType AnimType);

	// 스폰된 프로젝타일 발사
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LaunchSpawnedProjectile(ASLAIProjectile* ProjectileToLaunch, FVector TargetLocation, float ProjectileSpeed);
	
	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	TArray<ASLAIProjectile*> SpawnProjectileFanAtLocation(TSubclassOf<ASLAIProjectile> ProjectileClass, FVector TargetLocation, FName SocketName = NAME_None, int32 ProjectileCount = 5, float FanHalfAngle = 30.0f, float ProjectileSpeed = 2000.0f, EAttackAnimType AnimType = EAttackAnimType::AAT_Attack_01);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool GetIsTargetClose(float DistanceThreshold);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanBeExecuted() const;

	UFUNCTION(BlueprintCallable, Category = "Combat") 
	void PlayExecutionAnimation(EAttackAnimType ExecutionType, AActor* Executor);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AIJump();
    
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool CanAIJump() const;
    
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsJumping() const { return bIsJumping; }
    
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE bool IsLanding() const { return bIsLanding; }

	UFUNCTION(BlueprintCallable, Category = "combat")
	FORCEINLINE bool GetIsLoop() const { return bIsLoop; }

	UFUNCTION(BlueprintCallable, Category = "combat")
	void SetIsLoop(bool bNewLoop);

	UFUNCTION(BlueprintCallable, Category = "combat")
	FORCEINLINE bool GetIsAirHit() const { return bIsAirHit; }

	UFUNCTION(BlueprintCallable, Category = "combat")
	void SetIsAirHit(bool NewbIsAirHit);
	
	UFUNCTION(BlueprintCallable, Category = "combat")
	FORCEINLINE bool GetIsInvincibility() const { return IsInvincibility; }

	UFUNCTION(BlueprintCallable, Category = "combat")
	void SetIsInvincibility(bool NewIsInvincibility);
	
	UFUNCTION(BlueprintCallable, Category = "combat")
	void SetPreparedProjectile(ASLAIProjectile* NewPreparedProjectile);

	UFUNCTION(BlueprintCallable, Category = "combat")
	ASLAIProjectile* GetPreparedProjectile();

	UFUNCTION(BlueprintCallable)
	void SetHitReactionMode(EHitReactionMode NewMode);

	UFUNCTION(BlueprintCallable)
	void SetHitDamageThreshold(float NewThreshold);

	UFUNCTION(BlueprintCallable)
	void ResetAccumulatedDamage();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EHitReactionMode GetHitReactionMode() const { return HitReactionMode; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetHitDamageThreshold() const { return HitDamageThreshold; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAccumulatedDamage() const { return AccumulatedDamage; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDebugMode() const { return IsDebugMode; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeath();
    
	UFUNCTION(BlueprintCallable, Category = "Death")
	void HandleDeath();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool GetIsSpecialPattern() const { return bIsSpecialPattern; }
    
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetIsSpecialPattern(bool bNewIsSpecialPattern);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NotifyPatternFinished();

	FOnCharacterDeath OnCharacterDeath;
	FOnPatternFinished OnPatternFinished;
protected:
	
#if WITH_EDITOR
	//~ Begin UObject Interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif
	
	UFUNCTION(BlueprintCallable, Category = "Combat|Projectile")
	TArray<FVector> GenerateHorizontalFanDirections(const FVector& BaseDirection, int32 Count, float FanHalfAngle) const;

	UFUNCTION(BlueprintCallable)
	virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType);

	virtual void OnLanded(const FHitResult& Hit);

	bool ShouldPlayHitReaction(float DamageAmount);

	virtual void ProcessDeath();

	UFUNCTION(BlueprintCallable, Category = "Combat|Hit")
	virtual void SetHitState(bool bNewIsHit, float AutoResetTime = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ProcessDamageOnly(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType);
	
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

	// --- State Flags ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool IsHitReaction; // 피격 반응을 할건지 여부

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsDead; // 사망 상태인지 여부

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool IsDebugMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool IsInvincibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsAirHit;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "combat")
	EAttackAnimType CurrentAttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> HitEffectComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool bCanBeExecuted;  // 기본값은 true, 보스는 false로 설정

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Montages")
	TMap<EAttackAnimType, TObjectPtr<UAnimMontage>> ExecutionMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsBeingExecuted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsHit;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDown;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsStun;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAttacking;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bShouldLookAtPlayer;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EHitDirection HitDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector HitDirectionVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsJumping;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsLanding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsLoop;

	UPROPERTY(Transient)
	TObjectPtr<ASLAIProjectile> PreparedProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|HitReaction")
	EHitReactionMode HitReactionMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|HitReaction")
	float HitDamageThreshold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|HitReaction")
	float AccumulatedDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|HitReaction")
	TObjectPtr<UNiagaraSystem> DissolveEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsSpecialPattern;
};