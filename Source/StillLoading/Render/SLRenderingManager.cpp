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

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
    
	if (PropertyName.IsNone())
	{
		UpdateMaterialParameters();
		UpdatePostProcessRenderingType();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, CurrentRenderingType))
	{
		UpdatePostProcessRenderingType();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, PixelSize) ||
			 MemberPropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, Brightness) ||
			 MemberPropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, SolidOutlineSettings) ||
			 MemberPropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, SoftOutlineSettings))
	{
		UpdateMaterialParameters();
	}
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
    if (!MPCPixelization || !MPCSoftOutline || !MPCSolidOutline)
    {
        UE_LOG(LogTemp, Warning, TEXT("Material Parameter Collections are not set in %s"), *GetName());
        return;
    }
    
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCPixelization, FName(TEXT("PixelSize")), PixelSize);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCPixelization, FName(TEXT("Brightness")), Brightness);
    
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("MinKernelSize")), SolidOutlineSettings.MinKernelSize);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("MaxKernelSize")), SolidOutlineSettings.MaxKernelSize);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("MinDepthRange")), SolidOutlineSettings.MinDepthRange);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("MaxDepthRange")), SolidOutlineSettings.MaxDepthRange);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("NormalEdgeMinThreshold")), SolidOutlineSettings.NormalEdgeMinThreshold);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("NormalEdgeMaxThreshold")), SolidOutlineSettings.NormalEdgeMaxThreshold);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("DepthEdgeMinThreshold")), SolidOutlineSettings.DepthEdgeMinThreshold);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSolidOutline, FName(TEXT("DepthEdgeMaxThreshold")), SolidOutlineSettings.DepthEdgeMaxThreshold);
    
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSoftOutline, FName(TEXT("MinKernelSize")), SoftOutlineSettings.MinKernelSize);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSoftOutline, FName(TEXT("MaxKernelSize")), SoftOutlineSettings.MaxKernelSize);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSoftOutline, FName(TEXT("MinDepthRange")), SoftOutlineSettings.MinDepthRange);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSoftOutline, FName(TEXT("MaxDepthRange")), SoftOutlineSettings.MaxDepthRange);
    UKismetMaterialLibrary::SetScalarParameterValue(World, MPCSoftOutline, FName(TEXT("EdgeAlphaThreshold")), SoftOutlineSettings.EdgeAlphaThreshold);
}

void ASLRenderingManager::UpdatePostProcessRenderingType()
{
	check(PostProcess);
	check(PostProcess->Settings.WeightedBlendables.Array.Num() >= 3);

	PostProcess->Settings.WeightedBlendables.Array[0].Weight = CurrentRenderingType.bCelShaderFlag;
	PostProcess->Settings.WeightedBlendables.Array[1].Weight = CurrentRenderingType.bPixelArtFlag;
	PostProcess->Settings.WeightedBlendables.Array[2].Weight = CurrentRenderingType.bGrayScaleFlag;
}
