#pragma once

#include "CoreMinimal.h"
#include "Character/Widget/UserInputWidget.h"
#include "GameFramework/PlayerController.h"
#include "SLBasePlayerController.generated.h"

UCLASS()
class STILLLOADING_API ASLBasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserInputWidget> InputWidgetClass;

	UPROPERTY()
	UUserInputWidget* InputWidget;
};
