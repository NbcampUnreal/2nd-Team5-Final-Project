// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLInGameHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLInGameWidget.h"

void ASLInGameHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);

	InGameWidget = Cast<USLInGameWidget>(LevelWidgetObj);
	checkf(IsValid(InGameWidget), TEXT("Cast Fail. Level Widget To InGame Widget"));
}

void ASLInGameHUD::ApplyObjective(const FName& ObjectiveName)
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

void ASLInGameHUD::ApplyPlayerHp(int32 MaxHp)
{
	SetPlayerStateVisibility(true, false);
	SetPlayerHpValue(MaxHp, MaxHp);
}

void ASLInGameHUD::ApplyPlayerSpecial(int32 MaxValue)
{
	SetPlayerStateVisibility(true, true);
	SetPlayerSpecialValue(MaxValue, MaxValue);
}

void ASLInGameHUD::ApplyBossHp(int32 MaxHp)
{
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

void ASLInGameHUD::SetPlayerHpValue(int32 MaxHp, int32 CurrentHp)
{
	InGameWidget->SetHpValue(MaxHp, CurrentHp);
}

void ASLInGameHUD::SetPlayerSpecialValue(int32 MaxValue, int32 CurrentValue)
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

void ASLInGameHUD::SetBossHpValue(int32 MaxHp, int32 CurrentHp)
{
	InGameWidget->SetBossHpValue(MaxHp, CurrentHp);
}
