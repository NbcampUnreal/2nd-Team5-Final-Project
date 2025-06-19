#pragma once

#include "CoreMinimal.h"
#include "Controller/SLBasePlayerController.h"
#include "UObject/Interface.h"
#include "SLTargetableInterface.generated.h"

UINTERFACE(MinimalAPI)
class USLTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API ISLTargetableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	void OnHoveredByCursor(ASLBasePlayerController* InstigatingController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	void OnUnhoveredByCursor(ASLBasePlayerController* InstigatingController);
};