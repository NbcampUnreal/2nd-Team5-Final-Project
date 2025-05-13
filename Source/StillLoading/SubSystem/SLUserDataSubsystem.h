// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/SLUITypes.h"
#include "Character//DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "SLUserDataSubsystem.generated.h"

class USLUserDataSettings;
class UInputMappingContext;
class UGameUserSettings;
class URendererSettings;
class USLUISubsystem;

UCLASS()
class STILLLOADING_API USLUserDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void ApplyLoadedUserData();
	void ApplyDefaultUserData();

	void SetLanguage(ESLLanguageType NewType);
	void SetBgmVolume(float VolumeValue);
	void SetEffectVolume(float VolumeValue);
	void SetBrightness(float BrightnessValue);
	void SetWindowMode(int32 ModeValue);
	void SetScreenSize(const TPair<float, float>& SizeValue);
	
	ESLLanguageType GetCurrentLanguage() const;
	float GetCurrentBgmVolume() const;
	float GetCurrentEffectVolume() const;
	float GetCurrentBrightness() const;
	int32 GetCurrentWindowMode() const;
	TPair<float, float> GetCurrentScreenSize();

	bool UpdateMappingKey(EInputActionType TargetType, const FKey& KeyValue);
	const TMap<EInputActionType, FEnhancedActionKeyMapping>& GetActionKeyMap();
	TSet<FKey> GetKeySet() const;

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ApplyLanguageMode();
	void ApplyBgmVolume();
	void ApplyEffectVolume();
	void ApplyResolution();
	void ApplyBrightness();
	void ApplyWindowMode();

	void LoadKeyMapFromIMC();
	void LoadWidgetDataFromSaveSubSystem();

	void AddMappingDataToKeyMap(const FEnhancedActionKeyMapping& ActionKeyMapping);

	void CheckValidOfGameUserSettings();
	void CheckValidOfRendererSettings();
	void CheckValidOfUISubsystem();
	void CheckValidOfUserDataSettings();
	

private:
	UPROPERTY()
	const USLUserDataSettings* UserDataSettings = nullptr;

	UPROPERTY()
	TObjectPtr<UGameUserSettings> GameUserSettings = nullptr;

	UPROPERTY()
	TObjectPtr<URendererSettings> RendererSettings = nullptr;

	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> PlayerIMC = nullptr;

	UPROPERTY()
	TSet<FKey> KeySet;

	UPROPERTY()
	TMap<EInputActionType, FEnhancedActionKeyMapping> ActionKeyMap;

	UPROPERTY()
	ESLLanguageType LanguageType = ESLLanguageType::EL_Kor;

	UPROPERTY()
	float BgmVolume = 1.0f;

	UPROPERTY()
	float EffectVolume = 1.0f;

	UPROPERTY()
	float Brightness = 1.0f;

	UPROPERTY()
	int32 WindowMode = 0;

	UPROPERTY()
	float ScreenWidth = 1920.0f;

	UPROPERTY()
	float ScreenHeight = 1080.0f;
};
