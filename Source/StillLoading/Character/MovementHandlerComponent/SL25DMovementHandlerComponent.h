#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL25DMovementHandlerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL25DMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USL25DMovementHandlerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	
};
