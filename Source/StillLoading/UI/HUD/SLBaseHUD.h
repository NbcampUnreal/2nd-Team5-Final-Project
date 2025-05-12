// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/SLUITypes.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"
#include "SLBaseHUD.generated.h"

class USLLevelWidget;
class USLUISubsystem;

UCLASS()
class STILLLOADING_API ASLBaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetChapterToLevelWidget(ESLChapterType ChapterType);

	UFUNCTION(BlueprintCallable)
	void SetLanguageToLevelWidget(ESLLanguageType LanguageType);

protected:
	virtual void BeginPlay() override;
	virtual void OnStartedHUD() {};
	virtual void InitLevelWidget() {};

	void ApplyLevelWidgetInputMode();
	void CheckValidOfUISubsystem();
	void CheckValidOfLevelWidget();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TSubclassOf<USLLevelWidget> LevelWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetData")
	TMap<ESLChapterType, FSLLevelWidgetDataRow> ChapterWidgetDataMap;

protected:
	UPROPERTY()
	TObjectPtr<USLLevelWidget> LevelWidgetObj = nullptr;

	UPROPERTY()
	TObjectPtr<USLUISubsystem> UISubsystem = nullptr;

	ESLChapterType CurrentChapter = ESLChapterType::EC_Intro;
};
