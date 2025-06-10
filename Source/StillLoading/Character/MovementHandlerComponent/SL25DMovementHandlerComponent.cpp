#include "SL25DMovementHandlerComponent.h"


USL25DMovementHandlerComponent::USL25DMovementHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USL25DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USL25DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

