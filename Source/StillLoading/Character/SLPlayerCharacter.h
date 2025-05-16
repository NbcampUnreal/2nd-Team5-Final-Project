#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLBasePlayerCharacter.h"
#include "DataAsset/TagQueryDataAsset.h"
#include "SLPlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EQueryType : uint8 // enum class 는 앞에 안붙이는게 더 낫다
{
	EQT_MovementBlock UMETA(DisplayName = "Movement Block"),
	EQT_AttackBlock UMETA(DisplayName = "Attack Block"),
	EQT_JumpBlock UMETA(DisplayName = "Jump Block"),
	EQT_LookBlock UMETA(DisplayName = "Look Block"),
};

USTRUCT(BlueprintType)
struct FTagQueryAssetPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UTagQueryDataAsset>> QueryAssets;
};

UCLASS()
class STILLLOADING_API ASLCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsBlocking() const;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Movement")
	float LastLandTime = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, const FName SocketName) const;

	// Debug용 함수
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintPrimaryStateTags() const;
	
	UPROPERTY()
	TObjectPtr<AActor> Sword;
	UPROPERTY()
	TObjectPtr<AActor> Shield;
	
public:
	// 상태 태그 추가/제거 함수
	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void SetPrimaryState(FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void AddSecondaryState(FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void RemoveSecondaryState(FGameplayTag StateToRemove);

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

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void ClearAllPrimaryStates();

	UFUNCTION(BlueprintCallable, Category = "State Tags")
	void ClearAllSecondaryStates();

	// 상태 태그 컨테이너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer PrimaryStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer SecondaryStateTags;

	// 상태 조건 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	TMap<EQueryType, FTagQueryAssetPair> ConditionQueryMap;
};
