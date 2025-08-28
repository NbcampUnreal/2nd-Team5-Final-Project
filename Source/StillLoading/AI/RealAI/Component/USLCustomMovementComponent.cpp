// Fill out your copyright notice in the Description page of Project Settings.


#include "USLCustomMovementComponent.h"

#include "SLAILODComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"


// Sets default values for this component's properties
UUSLCustomMovementComponent::UUSLCustomMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUSLCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUSLCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
	{
		if (USLAILODComponent* LODComponent = MyCharacter->FindComponentByClass<USLAILODComponent>())
		{
			if (LODComponent->GetCurrentLODLevel() >= EAILODLevel::Medium)
			{
				return;
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

