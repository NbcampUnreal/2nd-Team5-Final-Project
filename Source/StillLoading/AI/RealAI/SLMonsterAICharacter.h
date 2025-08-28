#pragma once

#include "CoreMinimal.h"
#include "SLMonsterAICharacterBase.h"
#include "SLMonsterAICharacter.generated.h"

class ASLBasePlayerController;
class USLSoundSubsystem;
class USLBattleSoundSubsystem;
class UTimelineComponent;
class UWidgetComponent;
enum class EAttackAnimType : uint8;
class UBattleComponent;
class UAnimationMontageComponent;
class AAIController;
class UMonsterMeshDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentDied, AActor*, DiedMonster);

UCLASS()
class STILLLOADING_API ASLMonsterAICharacter : public ASLMonsterAICharacterBase
{
	GENERATED_BODY()

public:
	ASLMonsterAICharacter();

	UPROPERTY(BlueprintAssignable, Category = "Delegate | Battle")
	FOnAgentDied OnMonsterDied;

	UFUNCTION(BlueprintCallable)
	void ToggleWeaponState(bool bIsVisible);
	UFUNCTION(BlueprintCallable)
	void SpawnSpear();
	UFUNCTION(BlueprintCallable)
	void SpawnArrow();
	UFUNCTION()
	void BeginSpawning(const FVector& FinalLocation, float RiseHeight = 300.f);
	UFUNCTION()
	void HandleAnimNotify(EAttackAnimType MonsterMontageStage);
	UFUNCTION()
	void HandleHitNotify();
	UFUNCTION()
	void ActivateMovementComponent();
	
	// Anim
	void PlayAttackAnim();
	void PlayETCAnim();
	void PlayETCWaitAnim();

	UFUNCTION()
	void Dead(const AActor* Attacker, bool bIsChangeMaterial);
	UFUNCTION()
	void HandleAIPoolReturnOnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "On Death")
	void OnDeath();

	// 투척 각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SpearInaccuracy = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ThrowableClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;
	UPROPERTY()
	TObjectPtr<AActor> Throwable;

	// 피격시 BlendSpace 용
	UFUNCTION(BlueprintCallable)
	bool GetHit() const { return bIsHit; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	bool bIsHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float RetreatDistanceThreshold = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float RetreatDistance = 200.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnHoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController) override;
	virtual void OnUnhoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController) override;
	
	UFUNCTION()
	void ResetPushFlag();
	UFUNCTION()
	void UpdateSpawnMovement(float Alpha);
	UFUNCTION()
	void OnSpawnMovementFinished() const;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> SpawnTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TObjectPtr<UCurveFloat> SpawnMovementCurve;

	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<UMaterialInterface> HitMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Materials")
	TObjectPtr<UMaterialInterface> DeathMaterial;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, FName SocketName) const;
	UFUNCTION()
	void OnHitByCharacter(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						  FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType);
	UFUNCTION()
	void ChangeMeshTemporarily(float Rate = 0.3f);
	UFUNCTION()
	void ResetMaterial();
	UFUNCTION()
	void GetHitDirection(AActor* Causer);
	UFUNCTION()
	void RotateToHitCauser(const AActor* Causer);
	UFUNCTION()
	void FixCharacterVelocity();
	UFUNCTION(BlueprintCallable)
	void StartFlyingState();
	UFUNCTION(BlueprintCallable)
	void StopFlyingState();

	UFUNCTION()
	USLSoundSubsystem* GetBattleSoundSubSystem() const;
	void RecoverFromHitState();
	void PlayHitMontageAndSetupRecovery(float Length);

	UPROPERTY()
	TObjectPtr<AActor> LastAttacker;

	FTimerHandle PushResetHandle;
	FTimerHandle MaterialResetTimerHandle;
	FTimerHandle CollisionResetTimerHandle;
	FTimerHandle DeadTimerHandle;
	
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
};
