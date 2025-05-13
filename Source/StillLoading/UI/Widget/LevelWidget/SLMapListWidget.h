// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLMapListWidget.generated.h"

class UButton;
class UImage;
class UUniformGridPanel;
class USLMapElementWidget;

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLMapElementData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName MapName = "";

	UPROPERTY(EditAnywhere)
	FName ImageIndex = "";

	UPROPERTY(EditAnywhere)
	int32 TargetRow = 0;

	UPROPERTY(EditAnywhere)
	int32 TargetCol = 0;
};

UCLASS()
class STILLLOADING_API USLMapListWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType) override;
	virtual void DeactivateWidget() override;

protected:
	UFUNCTION()
	void OnClickedElement(ESLGameMapType TargetMapType);

	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

private:

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USLMapElementWidget> MapElementWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<ESLGameMapType, FSLMapElementData> MapElementDataMap;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel = nullptr;

	UPROPERTY()
	TMap<ESLGameMapType, USLMapElementWidget*> ElementMap;
};
