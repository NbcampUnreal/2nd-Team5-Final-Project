// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "Character//DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLUserDataSettings.generated.h"

class UInputMappingContext;

UCLASS(Config = UserDataSettings, DefaultConfig, meta = (DisplayName = "UserData Subsystem Settings"))
class STILLLOADING_API USLUserDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "DefaultIMC")
	TSoftObjectPtr<UInputMappingContext> PlayerIMC = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "ActionKeyType")
	TMap<TSoftObjectPtr<UInputAction>, EInputActionType> InputActionMap;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultLanguageSettings")
	ESLLanguageType DefaultLanguage = ESLLanguageType::EL_Kor;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultAudioSettings")
	float DefaultBgmVolume = 1.0f;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultAudioSettings")
	float DefaultEffectVolume = 1.0f;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultScreenSettings")
	float DefaultBrightness = 1.0f;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultScreenSettings")
	int32 DefaultWindowMode = 0;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultScreenSettings")
	float DefaultScreenWidth = 1920.0f;

	UPROPERTY(EditAnywhere, Config, Category = "DefaultScreenSettings")
	float DefaultScreenHeight = 1080.0f;
	
	UPROPERTY(EditAnywhere, Config, Category = "DefaultKeySettings")
	TMap<EInputActionType, FKey> DefaultActionKeyMap;
};
