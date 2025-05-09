#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
