#include "CombatHandlerComponent.h"

UCombatHandlerComponent::UCombatHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UCombatHandlerComponent::GetCurrentComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName) const
{
	if (!ComboDataAsset || ComboDataAsset->ComboChain.Num() == 0) return false;
	if (CurrentComboIndex >= ComboDataAsset->ComboChain.Num()) return false;

	const auto& [Montage, SectionName] = ComboDataAsset->ComboChain[CurrentComboIndex];
	OutMontage = Montage;
	OutSectionName = SectionName;
	
	return OutMontage != nullptr; 
}

bool UCombatHandlerComponent::GetNextComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName) const
{
	if (!ComboDataAsset || ComboDataAsset->ComboChain.Num() == 0) return false;

	int32 NextIndex = (CurrentComboIndex + 1) % ComboDataAsset->ComboChain.Num(); // Loop

	const FAttackComboData& ComboData = ComboDataAsset->ComboChain[NextIndex];
	OutMontage = ComboData.Montage;
	OutSectionName = ComboData.SectionName;
	
	return OutMontage != nullptr;
}

void UCombatHandlerComponent::AdvanceCombo()
{
	if (!ComboDataAsset || ComboDataAsset->ComboChain.Num() == 0) return;

	CurrentComboIndex++;
	if (CurrentComboIndex >= ComboDataAsset->ComboChain.Num())
	{
		ResetCombo();
	}
}