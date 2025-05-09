// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLBaseReactiveObject.h"
#include "Components\SphereComponent.h"
#include "StillLoading\Character\SLBaseCharacter.h"

// Sets default values
ASLBaseReactiveObject::ASLBaseReactiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
}

void ASLBaseReactiveObject::TriggerReact(ASLBaseCharacter* InCharacter, const ESLReactiveTriggerType InComingType)
{
	if (!IsValid(InCharacter)) return;
	if (!IsTriggerTypeAllowed(InComingType))
	{
		return;
	}

	OnReacted(InCharacter);
}



// Called when the game starts or when spawned
void ASLBaseReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBaseReactiveObject::OnReacted(const ASLBaseCharacter* InCharacter)
{

}

bool ASLBaseReactiveObject::IsTriggerTypeAllowed(ESLReactiveTriggerType InComingType)
{
	if (InComingType == ESLReactiveTriggerType::ERT_Both)
	{
		return true;
	}
	return InComingType == TriggerType;
}
