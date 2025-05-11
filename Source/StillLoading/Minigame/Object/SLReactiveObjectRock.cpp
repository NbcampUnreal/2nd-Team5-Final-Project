// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRock.h"
#include "StillLoading\Character\SLBaseCharacter.h"

void ASLReactiveObjectRock::OnReacted(const ASLBaseCharacter* InCharacter)
{
	checkf(IsValid(InCharacter), TEXT("InCharacter is nullptr!"));

	FVector RockLocation = GetActorLocation();

	FVector HitOrigin = InCharacter->GetActorLocation();
	FVector KnockbackDirection = (RockLocation - HitOrigin).GetSafeNormal();

	
	UPrimitiveComponent* Primitive = FindComponentByClass<UPrimitiveComponent>();
	if (Primitive && Primitive->IsSimulatingPhysics())
	{
		StaticMeshComp->SetSimulatePhysics(true);
		Primitive->AddImpulse(KnockbackDirection * KnockbackForce, NAME_None, true);
	}
}
