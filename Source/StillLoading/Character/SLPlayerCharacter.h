#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLPlayerCharacterBase.h"
#include "DataAsset/TagQueryDataAsset.h"
#include "SLPlayerCharacter.generated.h"

class ASLItem;
enum class EItemType : uint8;
enum class EOrbitDirection : uint8;

UENUM(BlueprintType)
enum class EQueryType : uint8 // enum class 는 앞에 안붙이는게 더 낫다
{
	EQT_MovementBlock UMETA(DisplayName = "Movement Block"),
	EQT_AttackBlock UMETA(DisplayName = "Attack Block"),
	EQT_JumpBlock UMETA(DisplayName = "Jump Block"),
	EQT_LookBlock UMETA(DisplayName = "Look Block"),
	EQT_AirBlock UMETA(DisplayName = "Air Block"),
	EQT_InputBlock UMETA(DisplayName = "Input Block"),
	EQT_DefenceBlock UMETA(DisplayName = "Defence Block"),
	EQT_LockOnBlock UMETA(DisplayName = "LockOn Block"),
	EQT_FacingBlock UMETA(DisplayName = "Facing Block"),
	EQT_DogeBlock UMETA(DisplayName = "Doge Block"),
};

USTRUCT(BlueprintType)
struct FTagQueryAssetPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UTagQueryDataAsset>> QueryAssets;
};

UCLASS()
class STILLLOADING_API ASLPlayerCharacter : public ASLPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASLPlayerCharacter();

	UFUNCTION()
	void EnableLockOnMode();
	UFUNCTION()
	void DisableLockOnMode();
	UFUNCTION()
	void BeginBlast(const EItemType ItemType);
	UFUNCTION()
	void OnEmpoweredStateChanged(bool bIsEmpowered);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASLItem> SwordClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASLItem> ShieldClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ASLItem> Sword;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ASLItem> Shield;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsBlocking() const;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Movement")
	float LastLandTime = 0.0f;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, const FName SocketName) const;

	UFUNCTION()
	void StartOrbitWithClone(const TSubclassOf<AActor>& ObjectClass, const EItemType ItemType, const EOrbitDirection OrbitDirection);

	// Debug용 함수
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintPrimaryStateTags() const;

	UFUNCTION()
	void OnItemSweeped(AActor* OverlappedActor, FHitResult Hit);
	
public:
	// 상태 태그 추가/제거 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(FGameplayTag NewState); // Update

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void AddSecondaryState(FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void RemoveSecondaryState(FGameplayTag StateToRemove); // Clear

	// 상태 검사 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsInPrimaryState(FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool HasSecondaryState(FGameplayTag StateToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	bool IsConditionBlocked(EQueryType QueryType) const;

	// 상태 태그 개별 제거 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void RemovePrimaryState(FGameplayTag StateToRemove);

	// 전체 제거
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void ClearAllStateTags();

	// 분류 제거
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void ClearStateTags(const TArray<FGameplayTag>& PrimaryExceptTagList, const TArray<FGameplayTag>& SecondaryExceptTagList);

	// 상태 태그 컨테이너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer PrimaryStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer SecondaryStateTags;

	// 상태 조건 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	TMap<EQueryType, FTagQueryAssetPair> ConditionQueryMap;

	// Spring Arm & Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera") 
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
};
