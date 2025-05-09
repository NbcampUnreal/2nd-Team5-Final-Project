#include "CameraManagerComponent.h"


UCameraManagerComponent::UCameraManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCameraManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

