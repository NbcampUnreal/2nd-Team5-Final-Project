// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectDoppelganger.h"
#include "Components\WidgetComponent.h"

ASLReactiveObjectDoppelganger::ASLReactiveObjectDoppelganger()
{
	ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
	ProgressWidget->SetupAttachment(StaticMeshComp);
	ProgressWidget->SetWidgetSpace(EWidgetSpace::World);
}

void ASLReactiveObjectDoppelganger::OnReacted(const ASLBaseCharacter* InCharacter)
{
}
