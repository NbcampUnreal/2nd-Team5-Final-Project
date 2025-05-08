#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/SLBaseCharacter.h"
#include "Components/Button.h"
#include "UserInputWidget.generated.h"

UCLASS()
class STILLLOADING_API UUserInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	
	UFUNCTION()
	void OnTempButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void StartRebind();

	UPROPERTY(meta = (BindWidget))
	UButton* TempButton;

	UPROPERTY()
	ASLBaseCharacter* LinkedCharacter;

private:
	bool bWaitingForInput = false;
};
