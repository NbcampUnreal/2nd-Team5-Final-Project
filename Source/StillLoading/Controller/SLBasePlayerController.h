#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Character/Widget/SLUserInputWidget.h"
#include "GameFramework/PlayerController.h"
#include "SLBasePlayerController.generated.h"

UCLASS()
class STILLLOADING_API ASLBasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	//~IGenericTeamAgentInterface inteface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//End of ~IGenericTeamAgentInterface inteface
protected:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserInputWidget> InputWidgetClass;

	UPROPERTY()
	UUserInputWidget* InputWidget;

private:
	FGenericTeamId playerTeamId;
};
