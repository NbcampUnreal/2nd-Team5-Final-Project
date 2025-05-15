#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChargingWidget.generated.h"

class UProgressBar;

UCLASS()
class STILLLOADING_API UChargingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetChargeProgress(const float Progress) const;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ChargeProgressBar;
};
