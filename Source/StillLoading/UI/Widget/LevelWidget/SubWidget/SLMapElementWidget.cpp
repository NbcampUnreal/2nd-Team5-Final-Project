// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SubWidget/SLMapElementWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void USLMapElementWidget::InitMapElement(ESLGameMapType NewType)
{
	ElementType = NewType;
	ElementButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedMapElementButton);
}

void USLMapElementWidget::SetMapElementImage(UTexture2D* ImageSource)
{
	ElementImg->SetBrushFromTexture(ImageSource);
}

void USLMapElementWidget::SetIsEnabledButton(bool bIsEndabled)
{
	ElementButton->SetIsEnabled(bIsEnabled);
}

void USLMapElementWidget::SetElementImgIndex(const FName& NewIndex)
{
	ImageIndex = NewIndex;
}

const FName& USLMapElementWidget::GetElementImgIndex()
{
	return ImageIndex;
}

void USLMapElementWidget::OnClickedMapElementButton()
{
	OnClickedMapElement.Broadcast(ElementType);
}
