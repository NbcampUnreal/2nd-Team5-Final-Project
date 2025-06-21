#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Boid/SwarmAgent.h"
#include "Character/BattleComponent/SLTargetableInterface.h"
#include "MonsterAICharacter.generated.h"

class ASLBasePlayerController;
class USLSoundSubsystem;
class USLBattleSoundSubsystem;
class AAISquadManager;
class UTimelineComponent;
class UWidgetComponent;
class USLMonsterStateTreeComponent;
enum class EAttackAnimType : uint8;
class UBattleComponent;
class UAnimationMontageComponent;
class AAIController;
class UMonsterMeshDataAsset;
class UFormationComponent;

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	MT_None 	UMETA(DisplayName = "None"),
	MT_SkullA	UMETA(DisplayName = "Skull A"),
	MT_SkullB	UMETA(DisplayName = "Skull B"),
	MT_SkullC	UMETA(DisplayName = "Skull C"),
	MT_SkullD	UMETA(DisplayName = "Skull D"),
	MT_SkullE	UMETA(DisplayName = "Skull E"),
	MT_SkullF	UMETA(DisplayName = "Skull F"),
	MT_SkullG	UMETA(DisplayName = "Skull G"),
	MT_SkullH	UMETA(DisplayName = "Skull H"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentDied, AActor*, DiedMonster);

UCLASS()
class STILLLOADING_API AMonsterAICharacter : public ASwarmAgent, public ISLTargetableInterface
{
	GENERATED_BODY()

public:
	AMonsterAICharacter();

	UPROPERTY(BlueprintAssignable, Category = "Delegate | Battle")
	FOnAgentDied OnMonsterDied;

	UFUNCTION()
	void SetSquadManager(AAISquadManager* InManager);

	UFUNCTION()
	void BeginSpawning(const FVector& FinalLocation, float RiseHeight = 300.f);

	UFUNCTION()
	void SetChasing(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(const FGameplayTag NewState);
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInPrimaryState(const FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetBattleState(const FGameplayTag NewState);
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool HasBattleState(const FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetStrategyState(const FGameplayTag NewState);
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool HasStrategyState(const FGameplayTag StateToCheck) const;
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void RemoveStrategyState();
	
	UFUNCTION()
	void HandleAnimNotify(EAttackAnimType MonsterMontageStage);

	UFUNCTION()
	void Dead(const AActor* Attacker, bool bIsChangeMaterial);

	UFUNCTION(BlueprintImplementableEvent, Category = "On Death")
	void OnDeath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EMonsterType CurrentMonsterType = EMonsterType::MT_None;
	// 상태 단일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer StateTags;
	// 배틀 상태 단일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer BattleStateTags;
	// 전략 상태 단일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer StrategyStateTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Squad")
	TObjectPtr<AAISquadManager> SquadManager;

	// 피격시 BlendSpace 용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float RetreatDistanceThreshold = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float RetreatDistance = 200.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnHoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController) override;
	virtual void OnUnhoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController) override;

	UFUNCTION()
	void OnHitByCharacter(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void ResetPushFlag();

	UFUNCTION()
	void UpdateSpawnMovement(float Alpha);
	UFUNCTION()
	void OnSpawnMovementFinished();

	UPROPERTY()
	TObjectPtr<UTimelineComponent> SpawnTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TObjectPtr<UCurveFloat> SpawnMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UAnimationMontageComponent> AnimationComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<UMaterialInterface> HitMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Materials")
	TObjectPtr<UMaterialInterface> DeathMaterial;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, FName SocketName) const;
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType);
	UFUNCTION()
	void ChangeMeshTemporarily(float Rate = 0.3f);
	UFUNCTION()
	void ResetMaterial();
	UFUNCTION()
	void HitDirection(AActor* Causer);
	UFUNCTION()
	void RotateToHitCauser(const AActor* Causer);
	UFUNCTION()
	void FixCharacterVelocity();
	UFUNCTION(BlueprintCallable)
	void StartFlyingState();
	UFUNCTION(BlueprintCallable)
	void StopFlyingState();
	UFUNCTION()
	void DrawDebugMessage();
	UFUNCTION()
	void StepBackward(const float DeltaTime);
	UFUNCTION()
	void WatchTarget(float DeltaTime);

	UFUNCTION()
	USLSoundSubsystem* GetBattleSoundSubSystem() const;

	UPROPERTY()
	TObjectPtr<AActor> LastAttacker;

	UPROPERTY()
	float MaxHealth;
	UPROPERTY()
	float CurrentHealth;

	UPROPERTY()
	FTimerHandle PushResetHandle;
	UPROPERTY()
	FTimerHandle MaterialResetTimerHandle;
	UPROPERTY()
	FTimerHandle CollisionResetTimerHandle;

	void RecoverFromHitState();
	void PlayHitMontageAndSetupRecovery(float Length);
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	bool bIsChasing = false;
	bool bIsLeader = false;
	bool bRecentlyPushed = false;
	bool bOriginalMaterialsInitialized = false;

	UPROPERTY()
	FVector SpawnStartLocation;
	UPROPERTY()
	FVector SpawnEndLocation;

public:
	FORCEINLINE TObjectPtr<UAnimationMontageComponent> GetAnimMontageComp()
	{
		if (AnimationComponent) return nullptr;
		return AnimationComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Stat")
	FORCEINLINE void SetMonsterMaxHealth(const float Health) { MaxHealth = Health; }

	FORCEINLINE AAISquadManager* GetSquadManager() const { return SquadManager; }
};
