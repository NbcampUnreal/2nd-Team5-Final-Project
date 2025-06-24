#include "CombatHandlerComponent.h"

#include "ChargingWidget.h"
#include "ChargingWidgetActor.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatHandlerComponent::UCombatHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ChargingWidgetActor = nullptr;
}

void UCombatHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());
	GenerateChargingWidget();

	if (bUnlimitedEmpowered)
	{
		SetUnlimitedEmpoweredCombatMode();
	}
}

void UCombatHandlerComponent::SetEmpoweredCombatMode(const float Duration)
{
	UE_LOG(LogTemp, Warning, TEXT("SetEmpoweredCombatMode [%f]"), Duration);
	OwnerCharacter->AddSecondaryState(TAG_Character_Empowered);

	OnEmpoweredStateChanged.Broadcast(true);

	float TotalDuration = Duration;
	TotalDuration = FMath::Min(TotalDuration, MaxEmpoweredDuration);

	if (CurrentMode == ECharacterComboState::CCS_Empowered && GetWorld()->GetTimerManager().IsTimerActive(CombatModeResetTimer))
	{
		const float Remaining = GetWorld()->GetTimerManager().GetTimerRemaining(CombatModeResetTimer);
		TotalDuration += Remaining;
	}

	CurrentMode = ECharacterComboState::CCS_Empowered;

	GetWorld()->GetTimerManager().ClearTimer(CombatModeResetTimer);
	GetWorld()->GetTimerManager().SetTimer(
		CombatModeResetTimer,
		this,
		&UCombatHandlerComponent::ResetCombatMode,
		TotalDuration,
		false
	);
}

void UCombatHandlerComponent::SetUnlimitedEmpoweredCombatMode()
{
	OwnerCharacter->AddSecondaryState(TAG_Character_Empowered);
	CurrentMode = ECharacterComboState::CCS_Empowered;
}

void UCombatHandlerComponent::ResetCombatMode()
{
	SetCombatMode(ECharacterComboState::CCS_Normal);
	GetWorld()->GetTimerManager().ClearTimer(CombatModeResetTimer);
	OwnerCharacter->RemoveSecondaryState(TAG_Character_Empowered);
	OnEmpoweredStateChanged.Broadcast(false);
}

void UCombatHandlerComponent::GenerateChargingWidget()
{
	if (!ChargingWidgetActorClass) return;
	if (!OwnerCharacter) return;

	ChargingWidgetActor = GetWorld()->SpawnActor<AChargingWidgetActor>(ChargingWidgetActorClass);
	if (ChargingWidgetActor)
	{
		ChargingWidgetActor->SetTargetCharacter(OwnerCharacter);
		ChargingWidgetActor->GetWidgetComponent()->SetVisibility(false);
	}
}

void UCombatHandlerComponent::StartCharging()
{
	if (bIsCharging || !ChargingWidgetActor) return;

	bIsCharging = true;
	ChargingElapsed = 0.f;

	if (UUserWidget* UserWidget = ChargingWidgetActor->GetWidgetComponent()->GetUserWidgetObject())
	{
		if (const UChargingWidget* ChargingWidget = Cast<UChargingWidget>(UserWidget))
		{
			ChargingWidget->SetChargeProgress(0.f);
		}
	}

	// 위젯 표시
	ChargingWidgetActor->GetWidgetComponent()->SetVisibility(true);

	// 충전 완료 Timer
	GetWorld()->GetTimerManager().SetTimer(ChargingFinishTimerHandle, this, &UCombatHandlerComponent::FinishCharging,
	                                       ChargingDuration, false);
	// Progress 업데이트 Timer (0.05초마다 호출)
	GetWorld()->GetTimerManager().SetTimer(ChargingUpdateTimerHandle, this,
	                                       &UCombatHandlerComponent::UpdateChargingProgress, 0.05f, true);
}

void UCombatHandlerComponent::UpdateChargingProgress()
{
	ChargingElapsed += 0.05f;
	const float Progress = FMath::Clamp(ChargingElapsed / ChargingDuration, 0.f, 1.f);

	if (UUserWidget* UserWidget = ChargingWidgetActor->GetWidgetComponent()->GetUserWidgetObject())
	{
		if (const UChargingWidget* ChargingWidget = Cast<UChargingWidget>(UserWidget))
		{
			ChargingWidget->SetChargeProgress(Progress);
		}
	}
}

void UCombatHandlerComponent::FinishCharging()
{
	bIsCharging = false;

	GetWorld()->GetTimerManager().ClearTimer(ChargingFinishTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ChargingUpdateTimerHandle);

	if (ChargingWidgetActor)
		ChargingWidgetActor->GetWidgetComponent()->SetVisibility(false);
}

void UCombatHandlerComponent::CancelCharging()
{
	if (!bIsCharging) return;
	bIsCharging = false;

	GetWorld()->GetTimerManager().ClearTimer(ChargingFinishTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ChargingUpdateTimerHandle);

	if (ChargingWidgetActor)
	{
		ChargingWidgetActor->GetWidgetComponent()->SetVisibility(false);
	}
}

// Combo
bool UCombatHandlerComponent::GetCurrentComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName)
{
	UAttackComboDataAsset* ActiveComboAsset;
	GetActiveComboDataAsset(ActiveComboAsset);

	if (!ActiveComboAsset || ActiveComboAsset->ComboChain.Num() == 0)
		return false;

	if (CurrentComboIndex >= ActiveComboAsset->ComboChain.Num())
		return false;

	const auto& [Montage, SectionName] = ActiveComboAsset->ComboChain[CurrentComboIndex];
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

void UCombatHandlerComponent::GetActiveComboDataAsset(UAttackComboDataAsset*& DataAsset)
{
	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		DataAsset = AirComboDataAsset;
	}
	else
	{
		switch (CurrentMode)
		{
		case ECharacterComboState::CCS_Normal:
			DataAsset = ComboDataAsset;
			break;
		case ECharacterComboState::CCS_Empowered:
			DataAsset = EmpoweredComboDataAsset;
			break;
		}
	}
}
