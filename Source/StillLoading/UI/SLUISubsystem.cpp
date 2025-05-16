// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SLUISubsystem.h"
#include "UI/SLUISettings.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "UI/Widget/AdditiveWidget/SLNotifyWidget.h"
#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "UI/Widget/AdditiveWidget/SLTalkWidget.h"
#include "Components/AudioComponent.h"

void USLUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USLUISubsystem::SetInputModeAndCursor()
{
	ESLInputModeType TargetInputMode = ESLInputModeType::EIM_UIOnly;
	bool bIsVisibleTargetCursor = true;

	if (ActiveAdditiveWidgets.IsEmpty())
	{
		TargetInputMode = CurrentLevelInputMode;
		bIsVisibleTargetCursor = bIsVisibleLevelCursor;
	}
	else
	{
		int32 ActiveCount = ActiveAdditiveWidgets.Num() - 1;

		TargetInputMode = ActiveAdditiveWidgets[ActiveCount]->GetWidgetInputMode();
		bIsVisibleTargetCursor = ActiveAdditiveWidgets[ActiveCount]->GetWidgetCursorMode();
	}

	APlayerController* CurrentPC = GetWorld()->GetPlayerControllerIterator()->Get();
	checkf(IsValid(CurrentPC), TEXT("Player Controller is invalid"));

	switch (CurrentLevelInputMode)
	{
	case ESLInputModeType::EIM_UIOnly:
		CurrentPC->SetInputMode(FInputModeUIOnly());
		break;

	case ESLInputModeType::EIM_GameOnly:
		CurrentPC->SetInputMode(FInputModeGameOnly());
		break;

	case ESLInputModeType::EIM_GameAndUI:
		CurrentPC->SetInputMode(FInputModeGameAndUI());
		break;
	}

	CurrentPC->SetShowMouseCursor(bIsVisibleTargetCursor);
}

void USLUISubsystem::SetChapterToUI(ESLChapterType ChapterType)
{
	CurrentChapter = ChapterType;

	for (USLAdditiveWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (IsValid(ActiveWidget))
		{
			ActiveWidget->ApplyOnChangedChapter(CurrentChapter);
		}
	}
}

void USLUISubsystem::SetLevelInputMode(ESLInputModeType InputModeType, bool bIsVisibleMouseCursor)
{
	CurrentLevelInputMode = InputModeType;
	bIsVisibleLevelCursor = bIsVisibleMouseCursor;
}

void USLUISubsystem::ActivateFade(bool bIsFadeIn)
{
	ESLAdditiveWidgetType FadeType = ESLAdditiveWidgetType::EAW_FadeWidget;
	CheckValidOfAdditiveWidget(FadeType);
	Cast<USLFadeWidget>(AdditiveWidgetMap[FadeType])->SetIsFadeIn(bIsFadeIn);
	AddAdditveWidget(FadeType);
}

void USLUISubsystem::ActivateNotify(ESLGameMapType MapType, ESLNotifyType NotiType)
{
	ESLAdditiveWidgetType NotifyType = ESLAdditiveWidgetType::EAW_NotifyWidget;
	CheckValidOfAdditiveWidget(NotifyType);
	AddAdditveWidget(NotifyType);
	Cast<USLNotifyWidget>(AdditiveWidgetMap[NotifyType])->UpdateNotifyText(MapType, NotiType);
}

void USLUISubsystem::ActivateStory(ESLStoryType TargetStoryType, int32 TargetIndex)
{
	ESLAdditiveWidgetType StoryType = ESLAdditiveWidgetType::EAW_StoryWidget;
	CheckValidOfAdditiveWidget(StoryType);
	Cast<USLStoryWidget>(AdditiveWidgetMap[StoryType])->UpdateStoryState(CurrentChapter, TargetStoryType, TargetIndex);
	AddAdditveWidget(StoryType);
}

void USLUISubsystem::ActivateTalk(ESLTalkTargetType TalkTargetType, int32 TargetIndex)
{
	ESLAdditiveWidgetType TalkType = ESLAdditiveWidgetType::EAW_TalkWidget;
	CheckValidOfAdditiveWidget(TalkType);
	Cast<USLTalkWidget>(AdditiveWidgetMap[TalkType])->UpdateTalkState(TalkTargetType, TargetIndex);
	AddAdditveWidget(TalkType);
}

void USLUISubsystem::AddAdditveWidget(ESLAdditiveWidgetType WidgetType)
{
	CheckValidOfAdditiveWidget(WidgetType);

	if (!ActiveAdditiveWidgets.Contains(AdditiveWidgetMap[WidgetType]))
	{
		ActiveAdditiveWidgets.Add(AdditiveWidgetMap[WidgetType]);

		AdditiveWidgetMap[WidgetType]->ActivateWidget(CurrentChapter);
		AdditiveWidgetMap[WidgetType]->AddToViewport(AdditiveWidgetMap[WidgetType]->GetWidgetOrder());

		SetInputModeAndCursor();
	}
}

void USLUISubsystem::RemoveCurrentAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (ActiveAdditiveWidgets.IsEmpty())
	{
		return;
	}

	if (AdditiveWidgetMap.Contains(WidgetType) &&
		!ActiveAdditiveWidgets.Contains(AdditiveWidgetMap[WidgetType]))
	{
		return;
	}

	AdditiveWidgetMap[WidgetType]->DeactivateWidget();
	ActiveAdditiveWidgets.Remove(AdditiveWidgetMap[WidgetType]);
	SetInputModeAndCursor();
}

void USLUISubsystem::RemoveAllAdditveWidget()
{
	for (USLAdditiveWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (IsValid(ActiveWidget))
		{
			ActiveWidget->DeactivateWidget();
		}
	}

	ActiveAdditiveWidgets.Empty();
	SetInputModeAndCursor();
}

void USLUISubsystem::PlayUISound(ESLUISoundType SoundType)
{
	CheckValidOfSoundSource(SoundType);

	if (!IsValid(AudioComp))
	{
		AudioComp = UGameplayStatics::CreateSound2D(GetGameInstance(), UISoundMap[SoundType], EffectVolume);
		AudioComp->bAutoDestroy = false;
	}

	StopUISound();

	AudioComp->SetSound(UISoundMap[SoundType]);
	AudioComp->Play();
}

void USLUISubsystem::StopUISound()
{
	if (IsValid(AudioComp) && AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}
}

const ESLChapterType USLUISubsystem::GetCurrentChapter() const
{
	return CurrentChapter;
}

const UDataTable* USLUISubsystem::GetImageDataTable()
{
	CheckValidOfImageDataTable();
	return WidgetImageData;
}

void USLUISubsystem::SetEffectVolume(float VolumeValue)
{
	EffectVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);

	if (IsValid(AudioComp))
	{
		AudioComp->SetVolumeMultiplier(EffectVolume);
	}
}

void USLUISubsystem::CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (AdditiveWidgetMap.Contains(WidgetType))
	{
		if (IsValid(AdditiveWidgetMap[WidgetType]))
		{
			return;
		}
	}

	CheckValidOfUISettings();
	checkf(UISettings->WidgetClassMap.Contains(WidgetType), TEXT("Widget Class Map is not contains widgettype"));

	UClass* WidgetClass = UISettings->WidgetClassMap[WidgetType].LoadSynchronous();
	checkf(IsValid(WidgetClass), TEXT("WidgetClass is invalid"));

	USLAdditiveWidget* TempWidget = CreateWidget<USLAdditiveWidget>(GetGameInstance(), WidgetClass);
	checkf(IsValid(TempWidget), TEXT("Fail Create Widget"));

	TempWidget->InitWidget(this, CurrentChapter);
	AdditiveWidgetMap.Add(WidgetType, TempWidget);
}

void USLUISubsystem::CheckValidOfUISettings()
{
	if (IsValid(UISettings))
	{
		return;
	}

	UISettings = GetDefault<USLUISettings>();
	checkf(IsValid(UISettings), TEXT("UI Settings is invalid"));
}

void USLUISubsystem::CheckValidOfSoundSource(ESLUISoundType SoundType)
{
	if (UISoundMap.Contains(SoundType))
	{
		if (IsValid(UISoundMap[SoundType]))
		{
			return;
		}
	}

	CheckValidOfUISettings();
	checkf(UISettings->WidgetSoundMap.Contains(SoundType), TEXT("Widget Sound Map is not contains soundtype"));

	USoundBase* SoundSource = UISettings->WidgetSoundMap[SoundType].LoadSynchronous();
	checkf(IsValid(SoundSource), TEXT("SoundSource is invalid"));

	UISoundMap.Add(SoundType, SoundSource);
}

void USLUISubsystem::CheckValidOfImageDataTable()
{
	if (IsValid(WidgetImageData))
	{
		return;
	}

	CheckValidOfUISettings();
	WidgetImageData = UISettings->WidgetDataTable.LoadSynchronous();
	checkf(IsValid(WidgetImageData), TEXT("Widget ImageData is invalid"));
}
