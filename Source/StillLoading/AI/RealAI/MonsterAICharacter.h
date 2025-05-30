#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "MonsterAICharacter.generated.h"

enum class EAttackAnimType : uint8;
class UBattleComponent;
class UAnimationMontageComponent;
class AAIController;
class UMonsterMeshDataAsset;

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

UCLASS()
class STILLLOADING_API AMonsterAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterAICharacter();

	UFUNCTION()
	void SetChasing(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(const FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInPrimaryState(const FGameplayTag StateToCheck) const;

	UFUNCTION()
	void HandleAnimNotify(EAttackAnimType MonsterMontageStage);

	UFUNCTION()
	void Dead(bool bWithMontage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EMonsterType CurrentMonsterType = EMonsterType::MT_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer StateTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

	// 피격시 BlendSpace 용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UAnimationMontageComponent> AnimationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBattleComponent> BattleComponent;
	
	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, FName SocketName) const;
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EAttackAnimType AnimType);
	UFUNCTION()
	void HitDirection(AActor* Causer);

	bool bIsChasing = false;
	bool bIsLeader = false;
};
