// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLSaveDataStructs.h"
#include "SLSaveGameSubsystem.generated.h"

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

    void SaveUserData();
    void OnSelectedNewGame();
    bool GetIsExistSaveData() const;

private:
    void LoadObjectiveDefaultData();
    
    void SaveChapterData();
    void SaveObjectiveData();
    
    void SendWidgetData();
    void SendChapterData();
    void SendObjectiveData();

    UPROPERTY()
    TObjectPtr<USLSaveGame> CurrentSaveData;

    UPROPERTY()
    FString SlotName = "SaveData";

    UPROPERTY()
    bool bIsExistSaveData = false;
};
