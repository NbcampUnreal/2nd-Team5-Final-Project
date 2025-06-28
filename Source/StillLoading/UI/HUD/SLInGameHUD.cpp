// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLInGameHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLInGameWidget.h"
#include "UI/Struct/SLInGameDelegateBuffers.h"
#include "GameMode/SLGameModeBase.h"
#include "Objective/SLObjectiveBase.h"

void ASLInGameHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);

	InGameWidget = Cast<USLInGameWidget>(LevelWidgetObj);
	checkf(IsValid(InGameWidget), TEXT("Cast Fail. Level Widget To InGame Widget"));

	ASLGameModeBase* GM = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode());

	if (IsValid(GM))
	{
		GM->OnPrimaryInProgressObjectiveChanged.AddDynamic(this, &ThisClass::OnAddObjective);
		GM->OnInProgressObjectiveRemoved.AddDynamic(this, &ThisClass::OnRemoveObjective);
	}
}

void ASLInGameHUD::OnAddObjective(USLObjectiveBase* TargetObjective)
{
	if (IsValid(TargetObjective))
	{
		FName ObjectiveName = TargetObjective->GetObjectiveName();
		int32 CompleteCount = TargetObjective->GetObjectiveCompleteCount();

		CurrentObjectiveName = ObjectiveName;

		if (CompleteCount == 1)
		{
			SetObjectiveName(ObjectiveName);
		}
		else
		{
			ObjectiveMaxCount = CompleteCount;
			SetObjectiveCounter(ObjectiveName, CompleteCount, 0);
			TargetObjective->OnObjectiveProgressChanged.AddUniqueDynamic(this, &ThisClass::OnObjectiveCountChanged);
		}
	}
}

void ASLInGameHUD::OnRemoveObjective(USLObjectiveBase* TargetObjective)
{
	if (TargetObjective->OnObjectiveProgressChanged.IsAlreadyBound(this, &ThisClass::OnObjectiveCountChanged))
	{
		TargetObjective->OnObjectiveProgressChanged.RemoveDynamic(this, &ThisClass::OnObjectiveCountChanged);
	}
}

void ASLInGameHUD::OnObjectiveCountChanged(int32 Count)
{
	SetObjectiveCounter(CurrentObjectiveName, ObjectiveMaxCount, Count);
}

void ASLInGameHUD::ApplyObjective()
{
	SetObjectiveVisibility();
}

void ASLInGameHUD::ApplyTimer(int32 SecondsValue)
{
	SetTimerVisibility(true);
	SetTimerValue(SecondsValue);
}

void ASLInGameHUD::ApplyPlayerHp(FSLPlayerHpDelegateBuffer& PlayerHpDelegate)
{
	bIsFirstApplyHp = true;

	if (!PlayerHpDelegate.OnPlayerHpChanged.IsAlreadyBound(this, &ThisClass::SetPlayerHpValue))
	{
		PlayerHpDelegate.OnPlayerHpChanged.AddDynamic(this, &ThisClass::SetPlayerHpValue);
	}
}

void ASLInGameHUD::ApplyPlayerSpecial(FSLSpecialValueDelegateBuffer& SpecialValueDelegate)
{
	if (!SpecialValueDelegate.OnSpecialValueChanged.IsAlreadyBound(this, &ThisClass::SetPlayerSpecialValue))
	{
		SpecialValueDelegate.OnSpecialValueChanged.AddDynamic(this, &ThisClass::SetPlayerSpecialValue);
	}
}

void ASLInGameHUD::ApplyBossHp(FSLBossHpDelegateBuffer& BossHpDelegate)
{
	if (!BossHpDelegate.OnBossHpChanged.IsAlreadyBound(this, &ThisClass::SetBossHpValue))
	{
		BossHpDelegate.OnBossHpChanged.AddDynamic(this, &ThisClass::SetBossHpValue);
	}

	SetBossStateVisibility(true);
	SetBossHpValue(100, 100);
}

void ASLInGameHUD::ApplyHitEffect(FSLPlayerHpDelegateBuffer& PlayerHpDelegate)
{
	bIsFirstApplyHp = true;

	if (!PlayerHpDelegate.OnPlayerHpChanged.IsAlreadyBound(this, &ThisClass::SetPlayerHpValue))
	{
		PlayerHpDelegate.OnPlayerHpChanged.AddDynamic(this, &ThisClass::SetPlayerHpValue);
	}

	SetHitEffectValue(100, 100);
}

bool ASLInGameHUD::GetIsActivated(ESLInGameActivateType TargetType)
{
	CheckValidOfLevelWidget();
	
	if (InGameWidget->GetActivateUIMap().Contains(TargetType))
	{
		return InGameWidget->GetActivateUIMap()[TargetType];
	}
	
	return false;
}

void ASLInGameHUD::SetTimerVisibility(bool bIsVisible)
{
	InGameWidget->SetIsTimerActivate(bIsVisible);
}

void ASLInGameHUD::SetPlayerStateVisibility(bool bIsVisible, bool bIsSpecial)
{
	InGameWidget->SetIsPlayerStateActivate(bIsVisible, bIsSpecial);
}

void ASLInGameHUD::SetVisibilityPlayerState(bool bIsVisible)
{
	InGameWidget->SetVisibilityPlayerStatePanel(bIsVisible);
	SetPlayerStateVisibility(GetIsActivated(ESLInGameActivateType::EIGA_PlayerHpBar), GetIsActivated(ESLInGameActivateType::EIGA_PlayerSpecialBar));
	SetPlayerHpValue(100, 100);
	SetPlayerSpecialValue(100, 0);
}

void ASLInGameHUD::SetObjectiveVisibility()
{
	InGameWidget->SetIsObjectiveActivate();
}

void ASLInGameHUD::SetInvisibleObjectivve()
{
	InGameWidget->SetInvisibleObjective();
}

void ASLInGameHUD::SetBossStateVisibility(bool bIsVisible)
{
	InGameWidget->SetIsBossStateActivate(bIsVisible);
}

void ASLInGameHUD::SetTimerValue(int32 SecondsValue)
{
	InGameWidget->SetTimerText(SecondsValue);
}

void ASLInGameHUD::SetPlayerHpValue(float MaxHp, float CurrentHp)
{
	if (!bIsFirstApplyHp)
	{
		SetHitEffectValue(MaxHp, CurrentHp);
	}
	else
	{
		bIsFirstApplyHp = false;
	}
	
	InGameWidget->SetHpValue(MaxHp, CurrentHp);
}

void ASLInGameHUD::SetPlayerSpecialValue(float MaxValue, float CurrentValue)
{
	InGameWidget->SetSpecialValue(MaxValue, CurrentValue);
}

void ASLInGameHUD::SetObjectiveName(const FName& ObjectiveName)
{
	InGameWidget->SetObjectiveText(ObjectiveName);
}

void ASLInGameHUD::SetObjectiveCounter(const FName& ObjectiveName, int32 MaxCount, int32 CurrentCount)
{
	InGameWidget->SetObjectiveByCounter(ObjectiveName, MaxCount, CurrentCount);
}

void ASLInGameHUD::SetBossHpValue(float MaxHp, float CurrentHp)
{
	InGameWidget->SetBossHpValue(MaxHp, CurrentHp);
}

void ASLInGameHUD::SetHitEffectValue(float MaxHp, float CurrentHp)
{
	InGameWidget->SetEffectValue(MaxHp, CurrentHp);
	InGameWidget->SetIsHitEffectActivate(true);
}
