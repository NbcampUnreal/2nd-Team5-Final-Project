// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SLBaseHUD.generated.h"

class USLLevelWidget;
class USLUISubsystem;

UCLASS()
class STILLLOADING_API ASLBaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OnPause();

	void OnUnpause();

protected:
	UFUNCTION()
	void OnChangedCurrentChapter(ESLChapterType ChapterType);

	virtual void BeginPlay() override;
	virtual void OnStartedHUD();
	virtual void InitLevelWidget() {};

	void ApplyLevelWidgetInputMode();
	void CheckValidOfUISubsystem();
	void CheckValidOfLevelWidget();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TSubclassOf<USLLevelWidget> LevelWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TSoftObjectPtr<UDataAsset> PrivateDataAsset = nullptr;

protected:
	UPROPERTY()
	TObjectPtr<USLLevelWidget> LevelWidgetObj = nullptr;

	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	UPROPERTY()
	FSLWidgetActivateBuffer ActivateBuffer;
};
