// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLSaveGameSubsystem.generated.h"

class USLSaveGame;

UCLASS()
class STILLLOADING_API USLSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    void SaveGame();

    UFUNCTION(BlueprintCallable)
    void LoadGame();

    UFUNCTION(BlueprintCallable)
    void NewGame();

    UFUNCTION(BlueprintCallable)
    USLSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }



private:

    UPROPERTY()
    USLSaveGame* CurrentSaveGame;

    UPROPERTY()
    FString SlotName = FString(TEXT("SaveData"));

};
