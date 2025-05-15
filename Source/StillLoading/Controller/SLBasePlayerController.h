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
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	FGenericTeamId playerTeamId;
};
