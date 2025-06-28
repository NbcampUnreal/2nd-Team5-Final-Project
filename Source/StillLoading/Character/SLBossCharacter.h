// SLBossCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "SLAIBaseCharacter.h"
#include "SLBossCharacter.generated.h"

// 보스의 고유 공격 패턴
UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	EBAP_None                UMETA(DisplayName = "None"), // 공격 없음 또는 기본 상태
	EBAP_Attack_01           UMETA(DisplayName = "Attack 01"),
	EBAP_Attack_02           UMETA(DisplayName = "Attack 02"),
	EBAP_Attack_03           UMETA(DisplayName = "Attack 03"),
	EBAP_Attack_04           UMETA(DisplayName = "Attack 04"),
	EBAP_DashAttack          UMETA(DisplayName = "Dash Attack"),
	EBAP_FootAttack_Left     UMETA(DisplayName = "Foot Attack - Left"),
	EBAP_FootAttack_Right    UMETA(DisplayName = "Foot Attack - Right"),
	EBAP_GroundSlam_01       UMETA(DisplayName = "Ground Slam 01"),
	EBAP_GroundSlam_02       UMETA(DisplayName = "Ground Slam 02"),
	EBAP_JumpAttack          UMETA(DisplayName = "Jump Attack"),
	EBAP_ThrowStone          UMETA(DisplayName = "Throw Stone"),
	EBAP_Whirlwind           UMETA(DisplayName = "Whirlwind Attack")
};

UCLASS()
class STILLLOADING_API ASLBossCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLBossCharacter();

	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBossAttackPattern(EBossAttackPattern NewPattern);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FindTargetPoint();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetTargetPointToBlackboard();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EBossAttackPattern SelectRandomPattern(float DistanceToTarget, const TArray<EBossAttackPattern>& CloseRangePatterns, const TArray<EBossAttackPattern>& LongRangePatterns,float DistanceThreshold = 500.0f);

	// 하위 스켈레탈 메시 머티리얼 관련 함수
	UFUNCTION(BlueprintCallable, Category = "Material", meta = (DisplayName = "Setup Child Meshes Materials"))
	void SetupChildMeshesMaterials(const TArray<USkeletalMeshComponent*>& ChildMeshes, int32 MaterialIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Material", meta = (DisplayName = "Set All Meshes Alpha"))
	void SetAllMeshesAlpha(float AlphaValue);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE EBossAttackPattern GetBossAttackPattern() const { return BossAttackPattern; }
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateAllMeshesFade();

private:
	// --- Combat Data ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossAttackPattern BossAttackPattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> TargetPoint;

	UPROPERTY()
	EBossAttackPattern LastLongRangePattern;

	// --- Material Data ---
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> ChildMeshesDynamicMaterials;

	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> ChildMeshComponents;

	UPROPERTY()
	TArray<float> ChildMeshesCurrentAlpha;

	UPROPERTY()
	TArray<float> ChildMeshesStartAlpha;

	UPROPERTY()
	FTimerHandle AllMeshesFadeTimerHandle;

	UPROPERTY()
	TMap<int32, FTimerHandle> IndividualFadeTimers;

	UPROPERTY()
	float AllMeshesTargetAlpha;

	UPROPERTY()
	float AllMeshesFadeStartTime;

	UPROPERTY()
	float AllMeshesFadeDuration;
};