// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLPauseWidget.generated.h"

class UImage;
class USLButtonWidget;

UCLASS()
class STILLLOADING_API USLPauseWidget : public USLAdditiveWidget
{
	GENERATED_BODY()

public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual bool ApplyOtherImage() override;
	virtual void OnEndedCloseAnim() override;

private:
	UFUNCTION()
	void OnClickedRestart();

	UFUNCTION()
	void OnClickedTitle();

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> RestartButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> CloseButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USLButtonWidget> TitleButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> LogoImg = nullptr;
};
