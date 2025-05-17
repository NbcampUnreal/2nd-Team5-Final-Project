// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectLuminousStatue.h"
#include "Components\PointLightComponent.h"


ASLReactiveObjectLuminousStatue::ASLReactiveObjectLuminousStatue()
{
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComp->SetupAttachment(StaticMeshComp);
	PointLightComp->SetRelativeLocation(FVector(0.f, 0.f, 0)); // 예시
}

void ASLReactiveObjectLuminousStatue::TurnOffLight()
{
	if (PointLightComp)
	{
		DeltaTime = 0;
		GetWorldTimerManager().SetTimer(LightControlHandler,
			[this]
			{
				DeltaTime += GetWorld()->GetDeltaSeconds() * 10;
				float Intensity = FMath::Lerp(PointLightComp->Intensity, 0, DeltaTime);
				PointLightComp->SetIntensity(Intensity);
				if (FMath::IsNearlyEqual(Intensity, 0, KINDA_SMALL_NUMBER))
				{
					UE_LOG(LogTemp, Warning, TEXT("End Timer"));
					DeltaTime = 0;
					GetWorld()->GetTimerManager().ClearTimer(LightControlHandler);
				}
			},
			0.05f, true);
	}
}

void ASLReactiveObjectLuminousStatue::BeginPlay()
{
	Super::BeginPlay();
}

void ASLReactiveObjectLuminousStatue::OnReacted(const ASLBaseCharacter* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	SetLightActive();
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, 
		[this] 
		{
			TurnOffLight();
		},
		5, false);
}

void ASLReactiveObjectLuminousStatue::SetLightActive()
{
	if (PointLightComp)
	{
		DeltaTime = 0;
		GetWorldTimerManager().SetTimer(LightControlHandler,
			[this]
			{
				DeltaTime += GetWorld()->GetDeltaSeconds() * 10;
				float Intensity = FMath::Lerp(0, 10000, DeltaTime);
				PointLightComp->SetIntensity(Intensity);
				if (FMath::IsNearlyEqual(Intensity, 0, KINDA_SMALL_NUMBER))
				{
					UE_LOG(LogTemp, Warning, TEXT("End Timer"));
					DeltaTime = 0;
					GetWorld()->GetTimerManager().ClearTimer(LightControlHandler);
				}
			},
			0.05f, true);
	}
}
