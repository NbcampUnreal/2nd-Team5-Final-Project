// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLSaveDataStructs.h"
#include "SLSaveGameSubsystem.generated.h"

class USLSaveGame;

UCLASS()
class STILLLOADING_API USLSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

 
    UFUNCTION(BlueprintCallable)
    void SaveGame();

    UFUNCTION(BlueprintCallable)
    void LoadGame();

    UFUNCTION(BlueprintCallable)
    void NewGame();

    UFUNCTION(BlueprintCallable)
    USLSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }

    UFUNCTION()
    const FWidgetSaveData& GetCurrentWidgetDataByRef() const;


private:
    UFUNCTION()
    void SaveWidgetData();

    UFUNCTION()
    void SendWidgetData();

    UFUNCTION()
    void SaveChapterData();

    UFUNCTION()
    void SendChapterData();


    UPROPERTY()
    USLSaveGame* CurrentSaveGame;

    UPROPERTY()
    FString SlotName = FString(TEXT("SaveData"));

};
