#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/AttackComboDataAsset.h"
#include "Components/ActorComponent.h"
#include "CombatHandlerComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterComboState : uint8
{
	CCS_Normal UMETA(DisplayName = "Normal"),
	CCS_Empowered UMETA(DisplayName = "Empowered"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCombatHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatHandlerComponent();
	
	UFUNCTION()
	bool GetCurrentComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName) const;
	UFUNCTION()
	bool GetNextComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName) const;
	UFUNCTION()
	void AdvanceCombo();
	UFUNCTION()
	void GetActiveComboDataAsset(UAttackComboDataAsset*& DataAsset) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttackComboDataAsset> ComboDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAttackComboDataAsset> EmpoweredComboDataAsset;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	int32 CurrentComboIndex = 0;
	UPROPERTY()
	ECharacterComboState CurrentMode = ECharacterComboState::CCS_Normal;

public:
	UFUNCTION()
	FORCEINLINE void ResetCombo() { CurrentComboIndex = 0; }
	UFUNCTION()
	FORCEINLINE int32 GetComboCount() const
	{
		return ComboDataAsset ? ComboDataAsset->ComboChain.Num() : 0;
	}
	UFUNCTION()
	FORCEINLINE void SetEmpowered(const ECharacterComboState Mode) { CurrentMode = Mode; }
	UFUNCTION()
	FORCEINLINE bool IsEmpowered() const { return CurrentMode == ECharacterComboState::CCS_Empowered; }
};
