// SLCompanionCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Character/SLAIBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "AI/DataTypes/SLCompanionDataTypes.h"
#include "SLCompanionCharacter.generated.h"


class USLCompanionFlyingComponent;
// Companion의 행동 패턴
UENUM(BlueprintType)
enum class ECompanionActionPattern : uint8
{
    // Common
    ECAP_None                    UMETA(DisplayName = "None"),
	ECAP_Attack_Plunge           UMETA(DisplayName = "Attack Plunge"),
	ECAP_Attack_Air              UMETA(DisplayName = "Attack Air"),

    // BM Attack Patterns
	ECAP_BM_MeleeCombo1          UMETA(DisplayName = "BM Melee Combo 1"),// BM 01->02->03->04->10
	ECAP_BM_MeleeCombo2          UMETA(DisplayName = "BM Melee Combo 2"),// BM 13->05->06->08->12
	ECAP_BM_MeleeCombo3          UMETA(DisplayName = "BM Melee Combo 3"),// BM 05->06->07->08->09
	ECAP_BM_QuickCombo1          UMETA(DisplayName = "BM Quick Combo 1"),// BM 10->11->12
	ECAP_BM_HeavyCombo1          UMETA(DisplayName = "BM Heavy Combo 1"),// BM 13->14->15->16
	ECAP_BM_FinisherCombo1       UMETA(DisplayName = "BM Finisher Combo 1"),// BM 17->18

    ECAP_BM_Attack01             UMETA(DisplayName = "BM Attack 01"),
    ECAP_BM_Attack02             UMETA(DisplayName = "BM Attack 02"),
    ECAP_BM_Attack03             UMETA(DisplayName = "BM Attack 03"),
    ECAP_BM_Attack04             UMETA(DisplayName = "BM Attack 04"),
    ECAP_BM_Attack05             UMETA(DisplayName = "BM Attack 05"),
    ECAP_BM_Attack06             UMETA(DisplayName = "BM Attack 06"),
    ECAP_BM_Attack07             UMETA(DisplayName = "BM Attack 07"),
    ECAP_BM_Attack08             UMETA(DisplayName = "BM Attack 08"),
    ECAP_BM_Attack09             UMETA(DisplayName = "BM Attack 09"),
    ECAP_BM_Attack10             UMETA(DisplayName = "BM Attack 10"),
    ECAP_BM_Attack11             UMETA(DisplayName = "BM Attack 11"),
    ECAP_BM_Attack12             UMETA(DisplayName = "BM Attack 12"),
    ECAP_BM_Attack13             UMETA(DisplayName = "BM Attack 13"),
    ECAP_BM_Attack14             UMETA(DisplayName = "BM Attack 14"),
    ECAP_BM_Attack15             UMETA(DisplayName = "BM Attack 15"),
    ECAP_BM_Attack16             UMETA(DisplayName = "BM Attack 16"),
    ECAP_BM_Attack17             UMETA(DisplayName = "BM Attack 17"),
    ECAP_BM_Attack18             UMETA(DisplayName = "BM Attack 18"),

    // WZ Attack Patterns
	ECAP_WZ_MeleeCombo1          UMETA(DisplayName = "WZ Melee Combo 1"),
	ECAP_WZ_MeleeCombo2          UMETA(DisplayName = "WZ Melee Combo 2"),
	ECAP_WZ_RangeCombo1          UMETA(DisplayName = "WZ Range Combo 1"),
	ECAP_WZ_RangeCombo2          UMETA(DisplayName = "WZ Range Combo 2"),
	
    ECAP_WZ_Attack01             UMETA(DisplayName = "WZ Attack 01"),
    ECAP_WZ_Attack02             UMETA(DisplayName = "WZ Attack 02"),
    ECAP_WZ_Attack03             UMETA(DisplayName = "WZ Attack 03"),
    ECAP_WZ_Attack04             UMETA(DisplayName = "WZ Attack 04"),
    ECAP_WZ_Attack05             UMETA(DisplayName = "WZ Attack 05"),
    ECAP_WZ_Attack06             UMETA(DisplayName = "WZ Attack 06"),
    ECAP_WZ_Attack07             UMETA(DisplayName = "WZ Attack 07"),
    ECAP_WZ_Attack08             UMETA(DisplayName = "WZ Attack 08"),
    ECAP_WZ_Attack09             UMETA(DisplayName = "WZ Attack 09"),
    ECAP_WZ_Attack10             UMETA(DisplayName = "WZ Attack 10"),
    ECAP_WZ_Attack11             UMETA(DisplayName = "WZ Attack 11"),
    ECAP_WZ_Attack12             UMETA(DisplayName = "WZ Attack 12"),
    ECAP_WZ_Attack13             UMETA(DisplayName = "WZ Attack 13"),
    ECAP_WZ_Attack14             UMETA(DisplayName = "WZ Attack 14"),
    ECAP_WZ_Attack15             UMETA(DisplayName = "WZ Attack 15"),
    ECAP_WZ_Attack16             UMETA(DisplayName = "WZ Attack 16"),
    ECAP_WZ_Attack17             UMETA(DisplayName = "WZ Attack 17"),
    ECAP_WZ_Attack18             UMETA(DisplayName = "WZ Attack 18"),
	ECAP_WZ_Attack19             UMETA(DisplayName = "WZ Attack 19"),
	ECAP_WZ_Attack20             UMETA(DisplayName = "WZ Attack 20"),
	ECAP_WZ_Attack21             UMETA(DisplayName = "WZ Attack 21"),
	ECAP_WZ_Attack22             UMETA(DisplayName = "WZ Attack 22"),
	ECAP_WZ_Attack23             UMETA(DisplayName = "WZ Attack 23"),

	ECAP_WZ_Loop_Attack04        UMETA(DisplayName = "WZ Loop Attack 04"),
	
	ECAP_WZ_Special_Patterns1    UMETA(DisplayName = "WZ Special Patterns1"),
	ECAP_WZ_Special_Patterns2    UMETA(DisplayName = "WZ Special Patterns2"),
	ECAP_WZ_Special_Patterns3    UMETA(DisplayName = "WZ Special Patterns3"),
	ECAP_WZ_Special_Patterns4    UMETA(DisplayName = "WZ Special Patterns4"),
	ECAP_WZ_Special_Patterns5    UMETA(DisplayName = "WZ Special Patterns5"),
	
	ECAP_WZ_Attack_Aim01             UMETA(DisplayName = "WZ Attack Aim 01"),
	ECAP_WZ_Attack_Aim02             UMETA(DisplayName = "WZ Attack Aim 02"),
	ECAP_WZ_Attack_Aim03             UMETA(DisplayName = "WZ Attack Aim 03"),
	ECAP_WZ_Attack_Aim04             UMETA(DisplayName = "WZ Attack Aim 04"),
};

UENUM(BlueprintType)
enum class ECompanionDistanceType : uint8
{
	ECDT_Melee                   UMETA(DisplayName = "Melee"),
	ECDT_MidRange                UMETA(DisplayName = "Mid Range"),
	ECDT_LongRange               UMETA(DisplayName = "Long Range"),
	ECDT_VeryLong                UMETA(DisplayName = "Very Long Range")
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLCompanionActionPatternMappingRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	ECompanionActionPattern ActionPattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	ECompanionDistanceType DistanceType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	bool bIsBattleMagePattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	bool bIsLoop;
	
	FSLCompanionActionPatternMappingRow();
};

USTRUCT(BlueprintType)
struct FNiagaraUserParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _ColorHue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _Size;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float  Z_Threashold;
};

USTRUCT(BlueprintType)
struct FNiagaraSpawnParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoDestroy;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoActivate;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENCPoolMethod PoolingMethod;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPreCullCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNiagaraUserParams UserParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Offset;
	FNiagaraSpawnParams();
};


class USLCompanionPatternData;

UCLASS()
class STILLLOADING_API ASLCompanionCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCompanionCharacter();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCombatMode(bool bInCombat);

	UFUNCTION(BlueprintCallable, Category = "Combat|GameplayTags")
	bool HasGameplayTag(const FGameplayTag& TagToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool GetIsBattleMage();

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetIsBattleMage(bool bInBattleMage);
    
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FGameplayTagContainer GetAvailablePatternsByDistance(float DistanceToTarget) const;

	// AI 태스크에서 사용할 랜덤 패턴 선택 함수들
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern SelectRandomPatternFromTags(const FGameplayTagContainer& PatternTags);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern SelectRandomPatternFromTagsWithDistance(const FGameplayTagContainer& PatternTags, float DistanceToTarget);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE ECompanionActionPattern GetCurrentActionPattern() const { return CurrentActionPattern; }
    
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCurrentActionPattern(ECompanionActionPattern NewPattern);

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	bool GetIsTeleporting() const;
	
	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void SetIsTeleporting(bool NewIsTeleporting);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformGroundExplosion(const FNiagaraSpawnParams& InWarningParams, const FNiagaraSpawnParams& InExplosionParams, float ExplosionRadius, EAttackAnimType AttackAnimType, 
		float WarningDuration = 1.0f, int32 HitCount = 1, float HitInterval = 0.2f);
	
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE USLCompanionFlyingComponent* GetFlyingComponent() const { return FlyingComponent; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	FORCEINLINE ASLBaseAIController* GetAIController() const { return AIController; }
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType AnimType) override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyExplosionDamage(FVector ExplosionLocation, float ExplosionRadius, EAttackAnimType AttackAnimType, bool bIsFirstHit = true);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	UNiagaraComponent* SpawnNiagaraEffect(const FNiagaraSpawnParams& SpawnParams);

	void ProcessMultiHit(const FMultiHitData& MultiHitData);

	UFUNCTION()
	void OnFlyingStateChanged(bool bIsFlying);

	virtual void ProcessDeath() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ASLAIProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName ProjectileSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsBattleMage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|GameplayTags")
	FGameplayTagContainer CurrentGameplayTags;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Distance")
	float MeleeRangeThreshold;
    
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Distance")
	float MidRangeThreshold;
    
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Distance")
	float LongRangeThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USLCompanionPatternData> PatternData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|GameplayTags")
	FGameplayTagContainer RecentlyUsedPatterns;
    
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxRecentPatternMemory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ECompanionActionPattern CurrentActionPattern;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USLCompanionFlyingComponent> FlyingComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	bool bIsInCombat;

	UPROPERTY()
	bool bIsTeleporting;

	UPROPERTY()
	TMap<FTimerHandle, FMultiHitData> ActiveMultiHits;
};