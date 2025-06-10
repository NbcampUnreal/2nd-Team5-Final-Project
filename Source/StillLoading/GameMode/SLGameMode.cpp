// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameMode.h"

#include "Kismet/GameplayStatics.h"

void ASLGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor) continue;

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

		for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
		{
			if (PrimComp)
			{
				PrimComp->SetGenerateOverlapEvents(true);
			}
		}
	}
}
