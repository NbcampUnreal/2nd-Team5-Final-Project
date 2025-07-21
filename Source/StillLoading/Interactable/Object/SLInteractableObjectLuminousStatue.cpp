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
	
	// 타이머 변수 초기화
	TransitionElapsedTime = 0.0f;
	TransitionTargetTime = TRANSITION_DURATION;
	TransitionStartIntensity = 0.0f;
	TransitionEndIntensity = 0.0f;
	bShouldUpdatePuzzleOnComplete = false;
}

void ASLInteractableObjectLuminousStatue::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 정리
	StopLightTransition();
	Super::EndPlay(EndPlayReason);
}

void ASLInteractableObjectLuminousStatue::TurnOffLight()
{
	StartLightTransition(MAX_INTENSITY, MIN_INTENSITY, false);
}

void ASLInteractableObjectLuminousStatue::BeginPlay()
{
	Super::BeginPlay();
	
	if (PointLightComp)
	{
		PointLightComp->SetIntensity(MIN_INTENSITY);
	}
	
	ASLMinigamePuzzleCond* PuzzleCond = FindActorInWorld<ASLMinigamePuzzleCond>(GetWorld());
	if (PuzzleCond)
	{
		PuzzleManager = PuzzleCond;
		UE_LOG(LogTemp, Warning, TEXT("Find PuzzleCond Succeed"));
		PuzzleManager->ObjectResetRequested.AddDynamic(this, &ASLInteractableObjectLuminousStatue::ResetCondition);
	}
}

void ASLInteractableObjectLuminousStatue::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	Super::OnInteracted(InCharacter, InTriggerType);
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
		UE_LOG(LogTemp, Error, TEXT("PointLightComp is null"));
		return;
	}
	
	StartLightTransition(MIN_INTENSITY, MAX_INTENSITY, true);
}

void ASLInteractableObjectLuminousStatue::StartLightTransition(float InStartIntensity, float InEndIntensity, bool bShouldUpdatePuzzle)
{
	// 기존 타이머 정리
	StopLightTransition();
	
	// 트랜지션 변수 설정
	TransitionElapsedTime = 0.0f;
	TransitionTargetTime = TRANSITION_DURATION;
	TransitionStartIntensity = InStartIntensity;
	TransitionEndIntensity = InEndIntensity;
	bShouldUpdatePuzzleOnComplete = bShouldUpdatePuzzle;
	
	// 타이머 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LightControlHandler,
			this,
			&ASLInteractableObjectLuminousStatue::UpdateLightIntensity,
			TRANSITION_INTERVAL,
			true
		);
	}
}

void ASLInteractableObjectLuminousStatue::UpdateLightIntensity()
{
	// 유효성 검사
	if (!PointLightComp || !IsValid(this))
	{
		StopLightTransition();
		return;
	}
	
	TransitionElapsedTime += TRANSITION_INTERVAL;
	float Alpha = FMath::Clamp(TransitionElapsedTime / TransitionTargetTime, 0.0f, 1.0f);
	float Intensity = FMath::Lerp(TransitionStartIntensity, TransitionEndIntensity, Alpha);
	
	PointLightComp->SetIntensity(Intensity);
	
	if (Alpha >= 1.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Light transition completed"));
		StopLightTransition();
		
		// 퍼즐 상태 업데이트
		if (bShouldUpdatePuzzleOnComplete && PuzzleManager.IsValid())
		{
			PuzzleManager->UpdateStatueState(StatueIndex, StatueIndex + 1);
		}
	}
}

void ASLInteractableObjectLuminousStatue::StopLightTransition()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LightControlHandler);
	}
}

void ASLInteractableObjectLuminousStatue::ResetCondition()
{
	TurnOffLight();
	bLightOn = false;
}