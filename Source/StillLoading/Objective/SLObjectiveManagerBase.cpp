// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/SLObjectiveManagerBase.h"
#include "Objective/SLObjectiveHandlerBase.h"
#include "UI/HUD/SLInGameHUD.h"

ASLObjectiveManagerBase::ASLObjectiveManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	ObjectiveHandler = CreateDefaultSubobject<USLObjectiveHandlerBase>(TEXT("Objective Handler"));
}

void ASLObjectiveManagerBase::ResetCurrentObjective()
{
	ResetCurrentGame();
}

void ASLObjectiveManagerBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (IsValid(PC))
	{
		HUD = Cast<ASLInGameHUD>(PC->GetHUD());
	}
}