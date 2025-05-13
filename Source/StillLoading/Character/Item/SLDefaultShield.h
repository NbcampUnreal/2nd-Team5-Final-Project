#pragma once

#include "CoreMinimal.h"
#include "SLItem.h"
#include "SLDefaultShield.generated.h"

class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLDefaultShield : public ASLItem
{
	GENERATED_BODY()

public:
	ASLDefaultShield();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;
};
