#pragma once

#include "CoreMinimal.h"
#include "SLItem.h"
#include "SLDefaultSword.generated.h"

class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLDefaultSword : public ASLItem
{
	GENERATED_BODY()

public:
	ASLDefaultSword();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;
};
