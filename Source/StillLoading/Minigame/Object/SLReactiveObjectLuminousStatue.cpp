// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectLuminousStatue.h"

#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Components\PointLightComponent.h"
#include "Minigame/System/SLMiniGameUtility.h"


ASLReactiveObjectLuminousStatue::ASLReactiveObjectLuminousStatue()
{
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComp->SetupAttachment(StaticMeshComp);
	PointLightComp->SetRelativeLocation(FVector(0.f, 0.f, 0)); // 예시
	bLightOn = false;
}

void ASLReactiveObjectLuminousStatue::TurnOffLight()
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

void ASLReactiveObjectLuminousStatue::BeginPlay()
{
	Super::BeginPlay();
	PointLightComp->SetIntensity(0.0f);
	ASLMinigamePuzzleCond* PuzzleCond = FindActorInWorld<ASLMinigamePuzzleCond>(GetWorld());

	if (PuzzleCond)
	{
		PuzzleManager = PuzzleCond;
		UE_LOG(LogTemp, Warning, TEXT("Find PuzzleCond Succeed"));
	}
	PuzzleManager->ObjectResetRequested.AddDynamic(this, &ASLReactiveObjectLuminousStatue::ResetCondition);
}

void ASLReactiveObjectLuminousStatue::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	if (!bLightOn)
	{
		SetLightActive();
		bLightOn = true;
	}
	
}

void ASLReactiveObjectLuminousStatue::SetLightActive()
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
				PuzzleManager->UpdateStatueState(StatueIndex, StatueIndex+1);
			}
		},
		Interval, true);
	
}

void ASLReactiveObjectLuminousStatue::ResetCondition()
{
	TurnOffLight();
	bLightOn = false;
}
