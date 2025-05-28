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
	UpdatePostProcessRenderingType();
	UpdateSolidOutlineSettings();
	UpdateSoftOutlineSettings();
	UpdateCelShaderRenderingType();
	UpdatePixelArtRenderingType();
}

#if WITH_EDITOR

void ASLRenderingManager::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, CurrentRenderingType))
	{
		UpdatePostProcessRenderingType();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, PixelArtSettings))
	{
		UpdatePixelArtRenderingType();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, SolidOutlineSettings))
	{
		UpdateSolidOutlineSettings();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, SoftOutlineSettings))
	{
		UpdateSoftOutlineSettings();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASLRenderingManager, CelShaderSettings))
	{
		UpdateCelShaderRenderingType();
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

void ASLRenderingManager::UpdatePostProcessRenderingType()
{
	check(PostProcess);
	check(PostProcess->Settings.WeightedBlendables.Array.Num() >= 3);

	PostProcess->Settings.WeightedBlendables.Array[0].Weight = CurrentRenderingType.bCelShaderFlag;
	PostProcess->Settings.WeightedBlendables.Array[1].Weight = CurrentRenderingType.bPixelArtFlag;
	PostProcess->Settings.WeightedBlendables.Array[2].Weight = CurrentRenderingType.bGrayScaleFlag;
}

void ASLRenderingManager::UpdatePixelArtRenderingType()
{
	check(MPCPixelization);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCPixelization, FName(TEXT("PixelSize")), PixelArtSettings.PixelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCPixelization, FName(TEXT("Brightness")), PixelArtSettings.Brightness);
}

void ASLRenderingManager::UpdateSolidOutlineSettings()
{
	check(MPCCelShader);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineMinKernelSize")), SolidOutlineSettings.MinKernelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineMaxKernelSize")), SolidOutlineSettings.MaxKernelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineMinDepthRange")), SolidOutlineSettings.MinDepthRange);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineMaxDepthRange")), SolidOutlineSettings.MaxDepthRange);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineNormalEdgeMinThreshold")), SolidOutlineSettings.NormalEdgeMinThreshold);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineNormalEdgeMaxThreshold")), SolidOutlineSettings.NormalEdgeMaxThreshold);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineDepthEdgeMinThreshold")), SolidOutlineSettings.DepthEdgeMinThreshold);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SolidOutlineDepthEdgeMaxThreshold")), SolidOutlineSettings.DepthEdgeMaxThreshold);
}

void ASLRenderingManager::UpdateSoftOutlineSettings()
{
	check(MPCCelShader);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SoftOutlineMinKernelSize")), SoftOutlineSettings.MinKernelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SoftOutlineMaxKernelSize")), SoftOutlineSettings.MaxKernelSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SoftOutlineMinDepthRange")), SoftOutlineSettings.MinDepthRange);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SoftOutlineMaxDepthRange")), SoftOutlineSettings.MaxDepthRange);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("SoftOutlineEdgeAlphaThreshold")), SoftOutlineSettings.EdgeAlphaThreshold);
}

void ASLRenderingManager::UpdateCelShaderRenderingType()
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCCelShader, FName(TEXT("ColorGrade")), CelShaderSettings.ColorGrade);
	UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MPCCelShader, FName(TEXT("LightColor")), CelShaderSettings.LightColor);
	UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MPCCelShader, FName(TEXT("ShadowColor")), CelShaderSettings.ShadowColor);
}
