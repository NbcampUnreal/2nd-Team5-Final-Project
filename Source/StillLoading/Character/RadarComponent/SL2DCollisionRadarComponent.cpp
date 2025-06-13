// Fill out your copyright notice in the Description page of Project Settings.


#include "SL2DCollisionRadarComponent.h"

#include "Character/SLPlayerCharacter.h"

USL2DCollisionRadarComponent::USL2DCollisionRadarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

const FVector USL2DCollisionRadarComponent::GetForwardVector()
{
	if (ASLPlayerCharacter* OwnerActor = Cast<ASLPlayerCharacter>(GetOwner()))
	{
		return OwnerActor->GetMesh()->GetRightVector();
	}
	return FVector::ZeroVector;
}