// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLInGameHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLInGameWidget.h"
#include "UI/Struct/SLInGameDelegateBuffers.h"

void ASLInGameHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);

	InGameWidget = Cast<USLInGameWidget>(LevelWidgetObj);
	checkf(IsValid(InGameWidget), TEXT("Cast Fail. Level Widget To InGame Widget"));
}

void ASLInGameHUD::ApplyObjective(const FName& ObjectiveName) // 게임 모드의 델리게이트 구독
{
	SetObjectiveName(ObjectiveName);
	SetObjectiveVisibility();
}

void ASLInGameHUD::ApplyObjectiveByCounter(const FName& ObjectiveName, int32 MaxCount)
{
	SetObjectiveCounter(ObjectiveName, MaxCount, 0);
	SetObjectiveVisibility();
}

void ASLInGameHUD::ApplyTimer(int32 SecondsValue)
{
	SetTimerVisibility(true);
	SetTimerValue(SecondsValue);
}

void ASLInGameHUD::ApplyPlayerHp(float MaxHp, FSLPlayerHpDelegateBuffer& PlayerHpDelegate)
{
	bIsFirstApplyHp = true;

	if (!PlayerHpDelegate.OnPlayerHpChanged.IsAlreadyBound(this, &ThisClass::SetPlayerHpValue))
	{
		PlayerHpDelegate.OnPlayerHpChanged.AddDynamic(this, &ThisClass::SetPlayerHpValue);
	}
	
	SetPlayerStateVisibility(true, false);
	SetPlayerHpValue(MaxHp, MaxHp);
}

void ASLInGameHUD::ApplyPlayerSpecial(float MaxValue, FSLSpecialValueDelegateBuffer& SpecialValueDelegate)
{
	if (!SpecialValueDelegate.OnSpecialValueChanged.IsAlreadyBound(this, &ThisClass::SetPlayerSpecialValue))
	{
		SpecialValueDelegate.OnSpecialValueChanged.AddDynamic(this, &ThisClass::SetPlayerSpecialValue);
	}
	
	SetPlayerStateVisibility(true, true);
	SetPlayerSpecialValue(MaxValue, 0);
}

void ASLInGameHUD::ApplyBossHp(float MaxHp, FSLBossHpDelegateBuffer& BossHpDelegate)
{
	if (!BossHpDelegate.OnBossHpChanged.IsAlreadyBound(this, &ThisClass::SetBossHpValue))
	{
		BossHpDelegate.OnBossHpChanged.AddDynamic(this, &ThisClass::SetBossHpValue);
	}

	SetBossStateVisibility(true);
	SetBossHpValue(MaxHp, MaxHp);
}

void ASLInGameHUD::ApplyHitEffect()
{
	InGameWidget->SetIsHitEffectActivate(true);
}

void ASLInGameHUD::SetTimerVisibility(bool bIsVisible)
{
	InGameWidget->SetIsTimerActivate(bIsVisible);
}

void ASLInGameHUD::SetPlayerStateVisibility(bool bIsVisible, bool bIsSpecial)
{
	InGameWidget->SetIsPlayerStateActivate(bIsVisible, bIsSpecial);
}

void ASLInGameHUD::SetObjectiveVisibility()
{
	InGameWidget->SetIsObjectiveActivate();
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
		ApplyHitEffect();
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
