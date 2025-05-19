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
				DeltaTime += GetWorld()->GetDeltaSeconds();
				float SetIntensity = FMath::Lerp(PointLightComp->Intensity, 0, DeltaTime * LightChangeSpeed);
				PointLightComp->SetIntensity(SetIntensity);
				if (PointLightComp->Intensity<=0)
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
	PointLightComp->SetIntensity(0.0f);
}

void ASLReactiveObjectLuminousStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
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
				DeltaTime += GetWorld()->GetDeltaSeconds();
				float Intensity = FMath::Lerp(0, 50, DeltaTime* LightChangeSpeed);
				PointLightComp->SetIntensity(Intensity);
				if (FMath::IsNearlyEqual(Intensity, 50.0f, KINDA_SMALL_NUMBER))
				{
					UE_LOG(LogTemp, Warning, TEXT("End Timer"));
					DeltaTime = 0;
					GetWorld()->GetTimerManager().ClearTimer(LightControlHandler);
				}
			},
			0.05f, true);
	}
}
