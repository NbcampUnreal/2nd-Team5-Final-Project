// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLDoppelganger.h"
#include "Components\WidgetComponent.h"
#include "Components\ProgressBar.h"
#include "Components\SceneCaptureComponent2D.h"
// Sets default values
ASLDoppelganger::ASLDoppelganger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(SceneComp);

	ProgressBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressBar"));
	ProgressBar->SetupAttachment(SceneComp);
	ProgressBar->SetWidgetSpace(EWidgetSpace::World);

	SceneCaptureCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureCamera->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void ASLDoppelganger::BeginPlay()
{
	Super::BeginPlay();
	
}

