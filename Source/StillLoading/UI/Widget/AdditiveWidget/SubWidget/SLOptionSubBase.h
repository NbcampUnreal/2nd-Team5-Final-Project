// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SLBaseWidget.h"
#include "SLOptionSubBase.generated.h"

UCLASS()
class STILLLOADING_API USLOptionSubBase : public USLBaseWidget
{
	GENERATED_BODY()
	
protected:
	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

	virtual bool ApplyOtherImage() override;
	virtual bool ApplySliderImage(FSliderStyle& SliderStyle);
	virtual bool ApplyExpandableImage(FSlateBrush& SlateBrush);
	virtual bool ApplyExpandedImage(FButtonStyle& ButtonStyle);
	virtual bool ApplyListBackImage(FSlateBrush& SlateBrush);
	virtual bool ApplyLeftArrowImage(FButtonStyle& ButtonStyle);
	virtual bool ApplyRightArrowImage(FButtonStyle& ButtonStyle);

	bool ApplyOptionButtonImage(FButtonStyle& ButtonStyle, ESLOptionPrivateImageType ButtonType);

protected:
	UPROPERTY()
	TMap<ESLOptionPrivateImageType, TObjectPtr<UObject>> PrivateImageMap;
};
