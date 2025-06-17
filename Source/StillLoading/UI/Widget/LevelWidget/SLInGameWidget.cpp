// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLInGameWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"

void USLInGameWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameOnly;
	bIsVisibleCursor = false;

	Super::InitWidget(NewUISubsystem);

}

void USLInGameWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	if (WidgetActivateBuffer.CurrentChapter == ESLChapterType::EC_Chapter2)
	{
		bIsVisibleCursor = true;
		WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	}

	Super::ActivateWidget(WidgetActivateBuffer);
}

void USLInGameWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void USLInGameWidget::SetIsTimerActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActiveTimerAnim, DeactiveTimerAnim);
}

void USLInGameWidget::SetIsPlayerStateActivate(bool bIsActived, bool bIsVisibleSpecial)
{
	if (bIsVisibleSpecial)
	{
		SpecialBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SpecialBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!PlayerStatePanel->IsVisible())
	{
		SetIsSubWidgetActivate(bIsActived, ActivePlayerStateAnim, DeactivePlayerStateAnim);
	}
}

void USLInGameWidget::SetIsObjectiveActivate()
{
	if (bIsObjectiveVisible)
	{
		SetIsSubWidgetActivate(false, ActiveGameStateAnim, DeactiveGameStateAnim);
	}
	else
	{
		SetIsSubWidgetActivate(true, ActiveGameStateAnim, DeactiveGameStateAnim);
	}

	bIsObjectiveVisible = !bIsObjectiveVisible;
}

void USLInGameWidget::SetIsBossStateActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActiveBossStateAnim, DeactiveBossStateAnim);
}

void USLInGameWidget::SetIsHitEffectActivate(bool bIsActived)
{
	if (bIsActived && bIsExistHitEffect)
	{
		PlaySubWidgetAnim(ActiveHitEffectAnim);
	}
	else
	{
		HitEffectPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLInGameWidget::SetTimerText(int32 TimeSeconds)
{
	if (TimeSeconds >= 0)
	{
		CurrentTimeValue = TimeSeconds;
		int32 TextMin = TimeSeconds / 60;
		int32 TextSec = TimeSeconds % 60;

		TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d : %02d"), TextMin, TextSec)));

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("SetTimerText"), TimeSeconds - 1);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
	}
	else
	{
		SetIsTimerActivate(false);
	}
	//FWidgetTransform TimerTransfrom = TimerHands->GetRenderTransform();
}

void USLInGameWidget::SetHpValue(float MaxHp, float CurrentHp)
{
	float PerHp = CurrentHp / MaxHp;

	HpBar->SetPercent(PerHp);
}

void USLInGameWidget::SetEffectValue(float MaxHp, float CurrentHp)
{
	float Opacity = 1 - (CurrentHp / MaxHp);

	UMaterialInterface* EffectMaterial = Cast<UMaterialInterface>(HitEffectImg->GetBrush().GetResourceObject());

	if (IsValid(EffectMaterial))
	{
		if (!IsValid(EffectDynamicMat))
		{
			EffectDynamicMat = UMaterialInstanceDynamic::Create(EffectMaterial, this);

			FSlateBrush NewBrush;
			NewBrush.SetResourceObject(EffectDynamicMat);
			HitEffectImg->SetBrush(NewBrush);
		}
		
		EffectDynamicMat->SetScalarParameterValue("MaxOpacity", Opacity);
	}
}

void USLInGameWidget::SetSpecialValue(float MaxValue, float CurrentValue)
{
	float PerHp = CurrentValue / MaxValue;

	SpecialBar->SetPercent(PerHp);
}

void USLInGameWidget::SetBossHpValue(float MaxHp, float CurrentHp)
{
	float PerHp = CurrentHp / MaxHp;

	BossHpBar->SetPercent(PerHp);
}

void USLInGameWidget::SetObjectiveText(const FName& ObjectiveName)
{
	CheckValidOfTextPoolSubsystem();
	const UDataTable* ObjectiveTextPool = TextPoolSubsystem->GetObjectiveTextPool();
	TArray<FSLObjectiveTextPoolDataRow*> TextPoolArray;
	ObjectiveTextPool->GetAllRows(TEXT("Obejctive Text Pool Context"), TextPoolArray);

	for (const FSLObjectiveTextPoolDataRow* ObjectiveText : TextPoolArray)
	{
		if (ObjectiveText->TextMap.Contains(ObjectiveName))
		{
			GameStateText->SetText(ObjectiveText->TextMap[ObjectiveName]);
		}
	}
}

void USLInGameWidget::SetObjectiveByCounter(const FName& ObjectiveName, int32 MaxCount, int32 CurrentCount)
{
	SetObjectiveText(ObjectiveName);

	FText CurrentText = GameStateText->GetText();
	FString TargetString = FString::Printf(TEXT("%s (%d / %d)"), *CurrentText.ToString(), CurrentCount, MaxCount);

	GameStateText->SetText(FText::FromString(TargetString));
}

const TMap<ESLInGameActivateType, bool>& USLInGameWidget::GetActivateUIMap()
{
	return ActivateUIMap;
}

void USLInGameWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLInGamePrivateDataAsset* PrivateData = Cast<USLInGamePrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetBrushDataMap();
		ActivateUIMap = PrivateData->GetActivateUIMap();
	}
}

void USLInGameWidget::ApplyFontData()
{
	Super::ApplyFontData();
}

bool USLInGameWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	TimerBack->SetBrush(SlateBrush);
	GameStateBack->SetBrush(SlateBrush);

	return true;
}

void USLInGameWidget::ApplyProgressBarImage()
{
	FProgressBarStyle ProgressBarStyle;
	FSlateBrush SlateBrush;

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_PlayerHpBack) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_PlayerHpBack]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_PlayerHpBack]);
		ProgressBarStyle.SetBackgroundImage(SlateBrush);
	}

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_PlayerHpBar) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_PlayerHpBar]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_PlayerHpBar]);
		ProgressBarStyle.SetFillImage(SlateBrush);
	}

	ProgressBarStyle.BackgroundImage.TintColor = HpBar->GetWidgetStyle().BackgroundImage.TintColor;
	ProgressBarStyle.FillImage.TintColor = HpBar->GetWidgetStyle().FillImage.TintColor;
	HpBar->SetWidgetStyle(ProgressBarStyle);

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_SpecailBack) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_SpecailBack]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_SpecailBack]);
		ProgressBarStyle.SetBackgroundImage(SlateBrush);
	}

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_SpecialBar) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_SpecialBar]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_SpecialBar]);
		ProgressBarStyle.SetFillImage(SlateBrush);
	}

	ProgressBarStyle.BackgroundImage.TintColor = SpecialBar->GetWidgetStyle().BackgroundImage.TintColor;
	ProgressBarStyle.FillImage.TintColor = SpecialBar->GetWidgetStyle().FillImage.TintColor;
	SpecialBar->SetWidgetStyle(ProgressBarStyle);

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_BossHpBack) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_BossHpBack]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_BossHpBack]);
		ProgressBarStyle.SetBackgroundImage(SlateBrush);
	}

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_BossHpBar) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_BossHpBar]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_BossHpBar]);
		ProgressBarStyle.SetFillImage(SlateBrush);
	}

	ProgressBarStyle.BackgroundImage.TintColor = BossHpBar->GetWidgetStyle().BackgroundImage.TintColor;
	ProgressBarStyle.FillImage.TintColor = BossHpBar->GetWidgetStyle().FillImage.TintColor;
	BossHpBar->SetWidgetStyle(ProgressBarStyle);
}

bool USLInGameWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_HitEffect) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_HitEffect]))
	{
		FSlateBrush SlateBrush;
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_HitEffect]);
		HitEffectImg->SetBrush(SlateBrush);
		bIsExistHitEffect = true;
	}
	else
	{
		bIsExistHitEffect = false;
	}

	ApplyTimerImage();
	ApplyProgressBarImage();

	return true;
}

void USLInGameWidget::SetIsSubWidgetActivate(bool bIsActived, UWidgetAnimation* ActiveAnim, UWidgetAnimation* DeactiveAnim)
{
	if (bIsActived)
	{
		PlaySubWidgetAnim(ActiveAnim, DeactiveAnim);
	}
	else
	{
		PlaySubWidgetAnim(DeactiveAnim, ActiveAnim);
	}
}

void USLInGameWidget::PlaySubWidgetAnim(UWidgetAnimation* PlayTargetAnim, UWidgetAnimation* StopTargetAnim)
{
	if (IsAnimationPlaying(PlayTargetAnim))
	{
		return;
	}

	if (IsValid(StopTargetAnim) && 
		IsAnimationPlaying(StopTargetAnim))
	{
		StopAnimation(StopTargetAnim);
	}

	PlayAnimation(PlayTargetAnim);
}

void USLInGameWidget::ApplyTimerImage()
{
	FSlateBrush SlateBrush;

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_TimerBorder) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_TimerBorder]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_TimerBorder]);
		TimerBorder->SetBrush(SlateBrush);
		TimerBorder->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TimerBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_TimerHands) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_TimerHands]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLInGamePrivateImageType::EGPI_TimerHands]);
		TimerHands->SetBrush(SlateBrush);
		TimerHands->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TimerHands->SetVisibility(ESlateVisibility::Collapsed);
	}
}