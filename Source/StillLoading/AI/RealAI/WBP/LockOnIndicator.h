#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockOnIndicator.generated.h"

class UImage;

UCLASS()
class STILLLOADING_API ULockOnIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> OutlineImage;

	void SetOutlineColor(const FLinearColor Color);
};
