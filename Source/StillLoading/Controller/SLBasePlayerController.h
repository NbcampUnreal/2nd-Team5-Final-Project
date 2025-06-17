#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "SLBasePlayerController.generated.h"

UCLASS()
class STILLLOADING_API ASLBasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void BindIMC(APawn* InPawn);

private:
	FGenericTeamId playerTeamId;
};
