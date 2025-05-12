// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLCreditTextWidget.generated.h"

class UTextBlock;

UCLASS()
class STILLLOADING_API USLCreditTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(const FText& TargetText);
	void SetTextFont(const FSlateFontInfo& TargetFont);

private:
	UPROPERTY(Meta = (BindWidget));
	TObjectPtr<UTextBlock> CreditText = nullptr;
};
