// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLUserDataSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/RendererSettings.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLUserDataSettings.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "InputMappingContext.h"
#include "SaveLoad/SLSaveDataStructs.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Kismet/GameplayStatics.h"

float USLUserDataSubsystem::GetCurrentScreenWidthSize()
{
	return ScreenWidth;
}

float USLUserDataSubsystem::GetCurrentScreenHeightSize()
{
	return ScreenHeight;
}

void USLUserDataSubsystem::ApplyLoadedUserData(const FWidgetSaveData& LoadData)
{
	SetLanguage(LoadData.LanguageType);
	SetBgmVolume(LoadData.BgmVolume);
	SetEffectVolume(LoadData.EffectVolume);
	SetWindowMode(LoadData.WindowMode);
	SetScreenSize({ LoadData.ScreenWidth, LoadData.ScreenHeight });
	SetBrightness(LoadData.Brightness);
	
	if (ActionKeyMap.IsEmpty())
	{
		LoadKeyMapFromIMC();
	}

	for (const TPair<EInputActionType, FEnhancedActionKeyMapping>& LoadedMapping : LoadData.ActionKeyMap)
	{
		ApplyMappingKey(LoadedMapping.Key, LoadedMapping.Value.Key);
	}
}

void USLUserDataSubsystem::ApplyDefaultUserData()
{
	CheckValidOfUserDataSettings();

	SetLanguage(UserDataSettings->DefaultLanguage);
	SetBgmVolume(UserDataSettings->DefaultBgmVolume);
	SetEffectVolume(UserDataSettings->DefaultEffectVolume);
	SetBrightness(UserDataSettings->DefaultBrightness);
	SetWindowMode(UserDataSettings->DefaultWindowMode);
	SetScreenSize({ UserDataSettings->DefaultScreenWidth, UserDataSettings->DefaultScreenHeight });

	if (ActionKeyMap.IsEmpty())
	{
		LoadKeyMapFromIMC();
	}

	for (const TPair<EInputActionType, FKey> ActionTypeKeyPair : UserDataSettings->DefaultActionKeyMap)
	{
		UpdateMappingKey(ActionTypeKeyPair.Key, ActionTypeKeyPair.Value);
	}
}

void USLUserDataSubsystem::SetLanguage(ESLLanguageType NewType)
{
	LanguageType = NewType;
	ApplyLanguageMode();
}

void USLUserDataSubsystem::SetBgmVolume(float VolumeValue)
{
	BgmVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);
	ApplyBgmVolume();
}

void USLUserDataSubsystem::SetEffectVolume(float VolumeValue)
{
	EffectVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);
	ApplyEffectVolume();
}

void USLUserDataSubsystem::SetBrightness(float BrightnessValue)
{
	Brightness = FMath::Clamp(BrightnessValue, 0.0f, 1.0f);
	ApplyBrightness();
}

void USLUserDataSubsystem::SetWindowMode(int32 ModeValue)
{
	WindowMode = ModeValue % 3;
	ApplyWindowMode();
}

void USLUserDataSubsystem::SetScreenSize(const TPair<float, float>& SizeValue)
{
	ScreenWidth = SizeValue.Key;
	ScreenHeight = SizeValue.Value;
	ApplyResolution();
}

ESLLanguageType USLUserDataSubsystem::GetCurrentLanguage() const
{
	return LanguageType;
}

float USLUserDataSubsystem::GetCurrentBgmVolume() const
{
	return BgmVolume;
}

float USLUserDataSubsystem::GetCurrentEffectVolume() const
{
	return EffectVolume;
}

float USLUserDataSubsystem::GetCurrentBrightness() const
{
	return Brightness;
}

int32 USLUserDataSubsystem::GetCurrentWindowMode() const
{
	return WindowMode;
}

TPair<float, float> USLUserDataSubsystem::GetCurrentScreenSize()
{
	return { ScreenWidth, ScreenHeight };
}

bool USLUserDataSubsystem::UpdateMappingKey(EInputActionType TargetType, const FKey& KeyValue)
{
	if (KeySet.Contains(KeyValue))
	{
		return false;
	}

	ApplyMappingKey(TargetType, KeyValue);

	return true;
}

const TMap<EInputActionType, FEnhancedActionKeyMapping>& USLUserDataSubsystem::GetActionKeyMap()
{
	if (ActionKeyMap.IsEmpty())
	{
		LoadKeyMapFromIMC();
	}

	return ActionKeyMap;
}

TSet<FKey> USLUserDataSubsystem::GetKeySet() const
{
	return KeySet;
}

void USLUserDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<USLUISubsystem>();
	Collection.InitializeDependency<USLTextPoolSubsystem>();
	Collection.InitializeDependency<USLSoundSubsystem>();

	Super::Initialize(Collection);

	CheckValidOfUserDataSettings();
	PlayerIMC = UserDataSettings->PlayerIMC.LoadSynchronous();
}

void USLUserDataSubsystem::ApplyLanguageMode()
{
	CheckValidOfTextPoolSubsystem();
	TextPoolSubsystem->OnChangedLanguage(LanguageType);
}

void USLUserDataSubsystem::ApplyBgmVolume()
{
	CheckValidOfSoundSubsystem();
	SoundSubsystem->SetBgmVolume(BgmVolume);
}

void USLUserDataSubsystem::ApplyEffectVolume()
{
	CheckValidOfSoundSubsystem();
	SoundSubsystem->SetEffectVolume(EffectVolume);
}

void USLUserDataSubsystem::ApplyResolution()
{
	CheckValidOfGameUserSettings();
	GameUserSettings->SetScreenResolution(FIntPoint(ScreenWidth, ScreenHeight));
	GameUserSettings->ApplySettings(false);
}

void USLUserDataSubsystem::ApplyBrightness()
{
	CheckValidOfRendererSettings();
	RendererSettings->DefaultFeatureAutoExposureBias = Brightness;

	FPropertyChangedEvent PropertyChangedEvent(URendererSettings::StaticClass()->
		FindPropertyByName(GET_MEMBER_NAME_CHECKED(URendererSettings, DefaultFeatureAutoExposureBias)));

	RendererSettings->PostEditChangeProperty(PropertyChangedEvent);
	RendererSettings->SaveConfig();
}

void USLUserDataSubsystem::ApplyWindowMode()
{
	CheckValidOfGameUserSettings();
	GameUserSettings->SetFullscreenMode((EWindowMode::Type)WindowMode);
	GameUserSettings->ApplySettings(false);
}

void USLUserDataSubsystem::ApplyMappingKey(EInputActionType TargetType, const FKey& KeyValue)
{
	if (ActionKeyMap.Contains(TargetType))
	{
		const UInputAction* TargetAction = ActionKeyMap[TargetType].Action;
		FKey OriginKey = ActionKeyMap[TargetType].Key;

		ActionKeyMap[TargetType].Key = KeyValue;

		KeySet.Remove(OriginKey);
		KeySet.Add(KeyValue);

		PlayerIMC->UnmapKey(TargetAction, OriginKey);
		PlayerIMC->MapKey(TargetAction, KeyValue);
	}
}

void USLUserDataSubsystem::LoadKeyMapFromIMC()
{
	checkf(IsValid(PlayerIMC), TEXT("IMC is invalid"));

	TArray<FEnhancedActionKeyMapping> ActionKeyMappings = PlayerIMC->GetMappings();

	for (const FEnhancedActionKeyMapping& ActionKeyMapping : ActionKeyMappings)
	{
		AddMappingDataToKeyMap(ActionKeyMapping);
	}
}

void USLUserDataSubsystem::AddMappingDataToKeyMap(const FEnhancedActionKeyMapping& ActionKeyMapping)
{
	EInputActionType ActionType = EInputActionType::EIAT_None;
	TSoftObjectPtr<UInputAction> TargetAction = ActionKeyMapping.Action;

	CheckValidOfUserDataSettings();
	
	if (!UserDataSettings->InputActionMap.Contains(TargetAction))
	{
		return;
	}

	ActionType = UserDataSettings->InputActionMap[TargetAction];
	KeySet.Add(ActionKeyMapping.Key);
	ActionKeyMap.Add(ActionType, ActionKeyMapping);
}

void USLUserDataSubsystem::CheckValidOfGameUserSettings()
{
	if (IsValid(GameUserSettings))
	{
		return;
	}

	GameUserSettings = GEngine->GetGameUserSettings();
	checkf(IsValid(GameUserSettings), TEXT("Game User Settings is invalid"));
}

void USLUserDataSubsystem::CheckValidOfRendererSettings()
{
	if (IsValid(RendererSettings))
	{
		return;
	}

	RendererSettings = GetMutableDefault<URendererSettings>();
	checkf(IsValid(RendererSettings), TEXT("Renderer Settings is invalid"));
}

void USLUserDataSubsystem::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return;
	}

	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	checkf(IsValid(UISubsystem), TEXT("UI Subsystem is invalid"));
}

void USLUserDataSubsystem::CheckValidOfTextPoolSubsystem()
{
	if (IsValid(TextPoolSubsystem))
	{
		return;
	}

	TextPoolSubsystem = GetGameInstance()->GetSubsystem<USLTextPoolSubsystem>();
	checkf(IsValid(TextPoolSubsystem), TEXT("TextPool Subsystem is invalid"));
}

void USLUserDataSubsystem::CheckValidOfSoundSubsystem()
{
	if (IsValid(SoundSubsystem))
	{
		return;
	}

	SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
}

void USLUserDataSubsystem::CheckValidOfUserDataSettings()
{
	if (IsValid(UserDataSettings))
	{
		return;
	}

	UserDataSettings = GetDefault<USLUserDataSettings>();
	checkf(IsValid(UserDataSettings), TEXT("User Data Settings is invalid"));
}
