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

void ASLInGameHUD::SetTimerVisibility(bool bIsVisible)
{
	InGameWidget->SetIsTimerActivate(bIsVisible);
}

void ASLInGameHUD::SetPlayerStateVisibility(bool bIsVisible)
{
	InGameWidget->SetIsPlayerStateActivate(bIsVisible);
}

void ASLInGameHUD::SetGameStateVisibility(bool bIsVisible)
{
	InGameWidget->SetIsGameStateActivate(bIsVisible);
}

void ASLInGameHUD::SetHitEffectVisibility(bool bIsVisible)
{
	InGameWidget->SetIsHitEffectActivate(bIsVisible);
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

void ASLInGameHUD::SetGameStateValue(int32 TargetState, const FText& NewText)
{
	// 
	InGameWidget->SetGameStateText(NewText);
}

void ASLInGameHUD::SetBossHpValue(int32 MaxHp, int32 CurrentHp)
{
	InGameWidget->SetBossHpValue(MaxHp, CurrentHp);
}
