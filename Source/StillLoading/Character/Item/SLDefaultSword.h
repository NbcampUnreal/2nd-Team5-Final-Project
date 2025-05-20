#pragma once

#include "CoreMinimal.h"
#include "SLItem.h"
#include "Character/Interface/OverlapToggleable.h"
#include "SLDefaultSword.generated.h"

class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLDefaultSword : public ASLItem, public IOverlapToggleable
{
	GENERATED_BODY()

public:
	ASLDefaultSword();

	virtual void EnableOverlap() override;
	virtual void DisableOverlap() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;
};
