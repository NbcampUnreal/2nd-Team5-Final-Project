// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/SLObjectiveManagerBase.h"

#include "SLObjectiveBase.h"
#include "Objective/SLObjectiveHandlerBase.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
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
	USLObjectiveBase* Objective =  ObjectiveHandler->GetObjective();

	if (Objective)
	{
		Objective->SetObjectiveProgressCount(0);
	}
	
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

void ASLObjectiveManagerBase::ActivatePlayerHpUI()
{
	GetPlayerState();

	if (IsValid(PlayerState))
	{
		HUD->ApplyPlayerHp(PlayerState->GetHPDelegate());

		if (!PlayerState->GetHPDelegate().OnPlayerHpChanged.IsAlreadyBound(this, &ThisClass::CheckPlayerDied))
		{
			PlayerState->GetHPDelegate().OnPlayerHpChanged.AddDynamic(this, &ThisClass::CheckPlayerDied);
		}
	}
}

void ASLObjectiveManagerBase::CheckPlayerDied(float MaxHp, float CurrentHp)
{
	if (CurrentHp <= 0)
	{
		PostPlayerDied();
	}
}

void ASLObjectiveManagerBase::GetPlayerState()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (IsValid(PC))
	{
		APlayerState* PS = PC->GetPawn()->GetPlayerState();

		if (IsValid(PS))
		{
			PlayerState = Cast<ASLBattlePlayerState>(PS);
		}
	}
}
