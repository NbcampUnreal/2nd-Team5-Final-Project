#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAILODComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAILODComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAILODComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
