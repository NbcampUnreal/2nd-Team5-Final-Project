// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLUserDataSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/RendererSettings.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLUserDataSettings.h"
#include "InputMappingContext.h"

void USLUserDataSubsystem::ApplyLoadedUserData()
{
	// TODO : Load User Data From SaveSubsystem

	ApplyLanguageMode();
	ApplyBgmVolume();
	ApplyEffectVolume();
	ApplyWindowMode();
	ApplyResolution();
	ApplyBrightness();
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

void USLUserDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CheckValidOfUserDataSettings();
	PlayerIMC = UserDataSettings->PlayerIMC.LoadSynchronous();
}

void USLUserDataSubsystem::ApplyLanguageMode()
{
	CheckValidOfUISubsystem();
	UISubsystem->SetLanguageToUI(LanguageType);
}

void USLUserDataSubsystem::ApplyBgmVolume()
{
	// TODO : Apply Bgm Volume To Bgm Audio Component
}

void USLUserDataSubsystem::ApplyEffectVolume()
{
	CheckValidOfUISubsystem();
	UISubsystem->SetEffectVolume(EffectVolume);
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

	if (ActionKeyMapping.Action->GetName().Contains(TEXT("MoveUp")))
	{
		ActionType = EInputActionType::EIAT_MoveUp;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("MoveDown")))
	{
		ActionType = EInputActionType::EIAT_MoveDown;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("MoveLeft")))
	{
		ActionType = EInputActionType::EIAT_MoveLeft;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("MoveRight")))
	{
		ActionType = EInputActionType::EIAT_MoveRight;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Walk")))
	{
		ActionType = EInputActionType::EIAT_Walk;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Jump")))
	{
		ActionType = EInputActionType::EIAT_Jump;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Attack")))
	{
		ActionType = EInputActionType::EIAT_Attack;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Interation")))
	{
		ActionType = EInputActionType::EIAT_Interaction;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("PointMove")))
	{
		ActionType = EInputActionType::EIAT_PointMove;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Menu")))
	{
		ActionType = EInputActionType::EIAT_Menu;
	}
	else if (ActionKeyMapping.Action->GetName().Contains(TEXT("Look")))
	{
		ActionType = EInputActionType::EIAT_Look;
	}

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

void USLUserDataSubsystem::CheckValidOfUserDataSettings()
{
	if (IsValid(UserDataSettings))
	{
		return;
	}

	UserDataSettings = GetDefault<USLUserDataSettings>();
	checkf(IsValid(UserDataSettings), TEXT("User Data Settings is invalid"));
}
