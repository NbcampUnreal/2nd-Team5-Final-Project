#include "SLAILODComponent.h"

USLAILODComponent::USLAILODComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USLAILODComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USLAILODComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

