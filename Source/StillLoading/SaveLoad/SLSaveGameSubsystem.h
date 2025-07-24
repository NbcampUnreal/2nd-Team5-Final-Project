// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLSaveDataStructs.h"
#include "SLSaveGameSubsystem.generated.h"

class USLSettingSaveGame;
struct FSLObjectiveRuntimeData;
enum class ESLChapterType : uint8;
class USLSaveGame;

UCLASS()
class STILLLOADING_API USLSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void SaveGameData();
    UFUNCTION(BlueprintCallable)
    void LoadGameData();
    UFUNCTION(BlueprintCallable)
    void ResetGameData();
    UFUNCTION(BlueprintCallable)
    void LoadSettingData();

    UFUNCTION(BlueprintCallable)
    void SaveGameDataByIndex(const int Index);
    UFUNCTION(BlueprintCallable)
    void LoadGameDataByIndex(const int Index);
    
    void SaveSettingData();
    void OnSelectedNewGame();
    bool GetIsExistSaveData() const;

private:
    void LoadObjectiveDefaultData();
    
    void SaveChapterData();
    void SaveSlotData();
    void SaveObjectiveData();
    
    void SendWidgetData();
    void SendChapterData();
    void SendObjectiveData();

private:
    UPROPERTY()
    TObjectPtr<USLSaveGame> CurrentGameSaveData;
    
    UPROPERTY()
    TObjectPtr<USLSettingSaveGame> SettingSaveData;

    TArray<FString> GameSaveSlotList = {"GameSaveSlot_1", "GameSaveSlot_2", "GameSaveSlot_3"};
    FString CurrentGameSlotName;
    FString SettingSlotName = "SettingSaveData";
    
    bool bIsExistSaveData = false;
    float StartSaveLoadTime = 0;
};
