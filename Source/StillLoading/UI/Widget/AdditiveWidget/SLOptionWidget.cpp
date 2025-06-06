// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLLanguageSettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLGraphicSettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLSoundSettingWidget.h"
#include "Animation/WidgetAnimation.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/SLBaseHUD.h"

const FName USLOptionWidget::TitleTextIndex = "TitleText";
const FName USLOptionWidget::KeySettingButtonIndex = "KeySettingButton";
const FName USLOptionWidget::LanguageSettingIndex = "LanguageSetting";
const FName USLOptionWidget::GraphicSettingIndex = "GraphicSetting";
const FName USLOptionWidget::SoundSettingIndex = "SoundSetting";
const FName USLOptionWidget::QuitGameButtonIndex = "QuitGameButton";
const FName USLOptionWidget::CloseButtonIndex = "CloseButton";

void USLOptionWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 15;
	bIsVisibleCursor = true;
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem);

	LanguageSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageSetting);
	GraphicSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedGraphicSetting);
	SoundSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedSoundSetting);
	KeySettingButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedKeySetting);
	QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuit);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);

	LanguageSetBt->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	GraphicSetBt->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	SoundSetBt->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	KeySettingButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	QuitGameButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	CloseButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);

	LanguageSettingWidget->InitWidget(NewUISubsystem);
	GraphicSettingWidget->InitWidget(NewUISubsystem);
	SoundSettingWidget->InitWidget(NewUISubsystem);
	KeySettingWidget->InitWidget(NewUISubsystem);
}

void USLOptionWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	if (IsValid(OpenAnim))
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		OnEndedOpenAnim();
	}

	LayerSwitcher->SetActiveWidgetIndex(0);
	
	LanguageSettingWidget->ActivateWidget(WidgetActivateBuffer);
	GraphicSettingWidget->ActivateWidget(WidgetActivateBuffer);
	SoundSettingWidget->ActivateWidget(WidgetActivateBuffer);
	KeySettingWidget->ActivateWidget(WidgetActivateBuffer);

	PlayUISound(ESLUISoundType::EUS_Open);
}

void USLOptionWidget::DeactivateWidget()
{
	if (IsValid(CloseAnim))
	{
		PlayAnimation(CloseAnim);
	}
	else
	{
		OnEndedCloseAnim();
	}

	PlayUISound(ESLUISoundType::EUS_Close);
}

void USLOptionWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Option)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex]);
	KeySettingText->SetText(OptionTextMap[KeySettingButtonIndex]);
	LanguageSetText->SetText(OptionTextMap[LanguageSettingIndex]);
	GraphicSetText->SetText(OptionTextMap[GraphicSettingIndex]);
	SoundSetText->SetText(OptionTextMap[SoundSettingIndex]);
	QuitGameText->SetText(OptionTextMap[QuitGameButtonIndex]);
	CloseText->SetText(OptionTextMap[CloseButtonIndex]);
}

bool USLOptionWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	KeySettingButton->SetStyle(ButtonStyle);
	QuitGameButton->SetStyle(ButtonStyle);
	CloseButton->SetStyle(ButtonStyle);
	LanguageSetBt->SetStyle(ButtonStyle);
	GraphicSetBt->SetStyle(ButtonStyle);
	SoundSetBt->SetStyle(ButtonStyle);

	return true;
}

bool USLOptionWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	OptionPanelBack->SetBrush(SlateBrush);

	return true;
}

void USLOptionWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	APlayerController* PC = GetWorld()->GetPlayerControllerIterator()->Get();
	checkf(IsValid(PC), TEXT("PlayerController is invalid"));

	ASLBaseHUD* HUD = Cast<ASLBaseHUD>(PC->GetHUD());
	checkf(IsValid(HUD), TEXT("HUD is invalid"));

	HUD->OnUnpause();
}

void USLOptionWidget::OnClickedLanguageSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(0);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedGraphicSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(1);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedSoundSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(2);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedKeySetting()
{
	LayerSwitcher->SetActiveWidgetIndex(3);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedQuit()
{
	PlayUISound(ESLUISoundType::EUS_Click);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}