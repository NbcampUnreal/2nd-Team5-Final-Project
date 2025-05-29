#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "MonsterAICharacter.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Tags")
	FGameplayTagContainer StateTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> SwordClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ShieldClass;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> Sword;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AActor> Shield;

private:
	UFUNCTION()
	void AttachItemToHand(AActor* ItemActor, FName SocketName) const;
	
	bool bIsChasing = false;
};
