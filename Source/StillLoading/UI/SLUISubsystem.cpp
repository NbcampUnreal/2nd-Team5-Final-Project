// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SLUISubsystem.h"
#include "UI/SLUISettings.h"
#include "UI/Widget/SLBaseWidget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

void USLUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	/*UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	GameUserSettings->ApplySettings(false);*/
}

void USLUISubsystem::SetChapterToUI(ESLChapterType ChapterType)
{
	CurrentChapter = ChapterType;

	for (USLBaseWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (!IsValid(ActiveWidget))
		{
			continue;
		}

		ActiveWidget->ApplyOnChangedChapter(CurrentChapter);
	}
}

void USLUISubsystem::SetLanguageToUI(ESLLanguageType LanguageType)
{
	CurrentLanguage = LanguageType;

	for (USLBaseWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (!IsValid(ActiveWidget))
		{
			continue;
		}

		ActiveWidget->ApplyOnChangedLanguage(LanguageType);
	}
}

void USLUISubsystem::AddAdditveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (!CheckValidOfAdditiveWidget(WidgetType))
	{
		return;
	}

	ActiveAdditiveWidgets.Add(AdditiveWidgetMap[WidgetType]);
	++ActiveCount;

	ActiveAdditiveWidgets[ActiveCount - 1]->ActivateWidget(CurrentChapter);
	ActiveAdditiveWidgets[ActiveCount - 1]->AddToViewport(ActiveCount);
}

void USLUISubsystem::RemoveCurrentAdditiveWidget()
{
	if (ActiveAdditiveWidgets.IsEmpty())
	{
		return;
	}

	if (!IsValid(ActiveAdditiveWidgets[ActiveCount - 1]))
	{
		return;
	}

	ActiveAdditiveWidgets[ActiveCount - 1]->DeactivateWidget();
	--ActiveCount;
}

void USLUISubsystem::RemoveAllAdditveWidget()
{
	for (USLBaseWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (!IsValid(ActiveWidget))
		{
			continue;
		}

		ActiveWidget->DeactivateWidget();
	}

	ActiveCount = 0;
}

void USLUISubsystem::PlayUISound(ESLUISoundType SoundType)
{
	if (!CheckValidOfSoundSource(SoundType))
	{
		return;
	}

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

const ESLLanguageType USLUISubsystem::GetCurrentLanguage() const
{
	return CurrentLanguage;
}

const UDataTable* USLUISubsystem::GetImageDataTable()
{
	if (!CheckValidOfImageDataTable())
	{
		return nullptr;
	}

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

bool USLUISubsystem::CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (AdditiveWidgetMap.Contains(WidgetType))
	{
		if (IsValid(AdditiveWidgetMap[WidgetType]))
		{
			return true;
		}
	}

	if (!CheckValidOfUISettings())
	{
		return false;
	}

	if (!UISettings->WidgetClassMap.Contains(WidgetType))
	{
		return false;
	}

	UClass* WidgetClass = UISettings->WidgetClassMap[WidgetType].LoadSynchronous();

	if (!IsValid(WidgetClass))
	{
		return false;
	}

	USLBaseWidget* TempWidget = CreateWidget<USLBaseWidget>(GetGameInstance(), WidgetClass);

	if (!IsValid(TempWidget))
	{
		return false;
	}

	TempWidget->InitWidget(this, CurrentChapter);
	AdditiveWidgetMap.Add(WidgetType, TempWidget);
	
	return true;
}

bool USLUISubsystem::CheckValidOfUISettings()
{
	if (IsValid(UISettings))
	{
		return true;
	}

	UISettings = GetDefault<USLUISettings>();

	if (!IsValid(UISettings))
	{
		return false;
	}

	return true;
}

bool USLUISubsystem::CheckValidOfSoundSource(ESLUISoundType SoundType)
{
	if (UISoundMap.Contains(SoundType))
	{
		if (IsValid(UISoundMap[SoundType]))
		{
			return true;
		}
	}

	if (!CheckValidOfUISettings())
	{
		return false;
	}

	if (!UISettings->WidgetSoundMap.Contains(SoundType))
	{
		return false;
	}

	USoundBase* SoundSource = UISettings->WidgetSoundMap[SoundType].LoadSynchronous();
	
	if (!IsValid(SoundSource))
	{
		return false;
	}

	UISoundMap.Add(SoundType, SoundSource);

	return true;
}

bool USLUISubsystem::CheckValidOfImageDataTable()
{
	if (IsValid(WidgetImageData))
	{
		return true;
	}

	if (!CheckValidOfUISettings())
	{
		return false;
	}

	WidgetImageData = UISettings->WidgetDataTable.LoadSynchronous();

	if (!IsValid(WidgetImageData))
	{
		return false;
	}

	return true;
}
