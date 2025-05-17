// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLMapListWidget.generated.h"

class UButton;
class UImage;
class UUniformGridPanel;
class USLMapElementWidget;

UCLASS()
class STILLLOADING_API USLMapListWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	UFUNCTION()
	void OnClickedElement(ESLGameMapType TargetMapType);

	virtual void ApplyFontData() override;
	virtual bool ApplyBackgroundImage() override;
	virtual bool ApplyOtherImage() override;

private:

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USLMapElementWidget> MapElementWidgetClass = nullptr;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel = nullptr;

	UPROPERTY()
	TMap<ESLGameMapType, USLMapElementWidget*> ElementMap;
};
