// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectLuminousStatue.h"

#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Components\PointLightComponent.h"
#include "Minigame/System/SLMiniGameUtility.h"


ASLInteractableObjectLuminousStatue::ASLInteractableObjectLuminousStatue()
{
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComp->SetupAttachment(StaticMeshComp);
	PointLightComp->SetRelativeLocation(FVector(0.f, 0.f, 0)); // 예시
	bLightOn = false;
}

void ASLInteractableObjectLuminousStatue::TurnOffLight()
{
	float ElapsedTime = 0.0f;

	const float Interval = 0.05f;

	const float TargetTime = 1.0f; // 변화 시간

	const float StartIntensity = 50.0f;

	const float EndIntensity = 0.0f;

	GetWorldTimerManager().SetTimer(LightControlHandler,
		[this, ElapsedTime, Interval, TargetTime, StartIntensity, EndIntensity]() mutable
		{
			ElapsedTime += Interval;
			float Alpha = FMath::Clamp(ElapsedTime / TargetTime, 0.0f, 1.0f);
			float Intensity = FMath::Lerp(StartIntensity, EndIntensity, Alpha);
			PointLightComp->SetIntensity(Intensity);

			if (Alpha >= 1.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("End Timer"));
				GetWorld()->GetTimerManager().ClearTimer(LightControlHandler);
			}
		},
		Interval, true);
}

void ASLInteractableObjectLuminousStatue::BeginPlay()
{
	Super::BeginPlay();
	PointLightComp->SetIntensity(0.0f);
	ASLMinigamePuzzleCond* PuzzleCond = FindActorInWorld<ASLMinigamePuzzleCond>(GetWorld());

	if (PuzzleCond)
	{
		PuzzleManager = PuzzleCond;
		UE_LOG(LogTemp, Warning, TEXT("Find PuzzleCond Succeed"));
		PuzzleManager->ObjectResetRequested.AddDynamic(this, &ASLInteractableObjectLuminousStatue::ResetCondition);
	}
	
}

void ASLInteractableObjectLuminousStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (!bLightOn)
	{
		SetLightActive();
		bLightOn = true;
	}
	
}

void ASLInteractableObjectLuminousStatue::SetLightActive()
{
	if (!PointLightComp)
	{
		return;
	}
	float ElapsedTime = 0.0f;

	const float Interval = 0.05f;

	const float TargetTime = 1.0f; // 변화 시간

	const float StartIntensity = 0.0f;

	const float EndIntensity = 50.0f;

	GetWorldTimerManager().SetTimer(LightControlHandler,
		[this, ElapsedTime, Interval, TargetTime, StartIntensity, EndIntensity]() mutable
		{
			ElapsedTime += Interval;
			float Alpha = FMath::Clamp(ElapsedTime / TargetTime, 0.0f, 1.0f);
			float Intensity = FMath::Lerp(StartIntensity, EndIntensity, Alpha);
			PointLightComp->SetIntensity(Intensity);

			if (Alpha >= 1.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("End Timer"));
				GetWorld()->GetTimerManager().ClearTimer(LightControlHandler);
				if (PuzzleManager)
				{
					PuzzleManager->UpdateStatueState(StatueIndex, StatueIndex + 1);
				}
				
			}
		},
		Interval, true);
	
}

void ASLInteractableObjectLuminousStatue::ResetCondition()
{
	TurnOffLight();
	bLightOn = false;
}
