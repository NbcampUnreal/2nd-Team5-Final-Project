// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLSplineTrack.h"
#include "Components/SplineComponent.h"

// Sets default values
ASLSplineTrack::ASLSplineTrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComp);
	SplineComp->SetClosedLoop(false);
	SplineComp->SetDrawDebug(true);
}
