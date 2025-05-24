// Fill out your copyright notice in the Description page of Project Settings.


#include "SLRenderingManager.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

ASLRenderingManager::ASLRenderingManager()
{
	PrimaryActorTick.bCanEverTick = false;

	InitComponents();
}

void ASLRenderingManager::BeginPlay()
{
	Super::BeginPlay();
	
}

#if WITH_EDITOR

void ASLRenderingManager::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdateMaterialParameters();
	UpdatePostProcessRenderingType();
}

#endif

void ASLRenderingManager::InitComponents()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(RootComponent);
	
	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessVolume"));
	PostProcess->SetupAttachment(RootComponent);
	
	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));
	DirectionalLight->SetupAttachment(RootComponent);
	
}

void ASLRenderingManager::UpdateMaterialParameters()
{
	check(MPCPixelization && MPCOutline);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCPixelization, FName(TEXT("PixelSize")), PixelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCPixelization, FName(TEXT("Brightness")), Brightness);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCOutline, FName(TEXT("SolidOutlineThickness")), SolidOutlineThickness);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCOutline, FName(TEXT("TransparentOutlineThickness")), TransparentOutlineThickness);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCOutline, FName(TEXT("DepthViewRange")), DepthViewRange);
}

void ASLRenderingManager::UpdatePostProcessRenderingType()
{
	check(PostProcess);
	check(PostProcess->Settings.WeightedBlendables.Array.Num() >= 3);

	PostProcess->Settings.WeightedBlendables.Array[0].Weight = CurrentRenderingType.bCelShaderFlag;
	PostProcess->Settings.WeightedBlendables.Array[1].Weight = CurrentRenderingType.bPixelArtFlag;
	PostProcess->Settings.WeightedBlendables.Array[2].Weight = CurrentRenderingType.bGrayScaleFlag;
}
