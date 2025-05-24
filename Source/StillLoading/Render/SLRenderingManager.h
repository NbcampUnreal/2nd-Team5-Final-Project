// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLRenderingManager.generated.h"

class UPostProcessComponent;

USTRUCT(BlueprintType)
struct FRenderingType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, DisplayName="카툰 렌더링 적용", Category = "Flags")
	bool bCelShaderFlag = false;

	UPROPERTY(EditAnywhere, DisplayName="픽셀 아트 렌더링 적용", Category = "Flags")
	bool bPixelArtFlag = false;

	UPROPERTY(EditAnywhere, DisplayName="흑백 렌더링 적용", Category = "Flags")
	bool bGrayScaleFlag = false;
};

UCLASS(Blueprintable)
class STILLLOADING_API ASLRenderingManager : public AActor
{
	GENERATED_BODY()

public:
	ASLRenderingManager();
	
protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	void InitComponents();
	void UpdateMaterialParameters();
	void UpdatePostProcessRenderingType();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLRendering", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialParameterCollection> MPCOutline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLRendering", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialParameterCollection> MPCPixelization;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPostProcessComponent> PostProcess;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDirectionalLightComponent> DirectionalLight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkyLightComponent> SkyLight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering", DisplayName="렌더링 타입 토글", meta = (AllowPrivateAccess = "true"))
	FRenderingType CurrentRenderingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|픽셀 아트 렌더링", DisplayName= "픽셀화 크기", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1024"))
	int32 PixelSize = 512;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|픽셀 아트 렌더링", DisplayName="흑백 명도", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float Brightness = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|아웃라인", DisplayName="외곽선 렌더링 범위", meta = (AllowPrivateAccess = "true", ClampMin = "500.0", ClampMax = "20000.0"))
	float DepthViewRange = 10000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|아웃라인", DisplayName="불투명 외곽선 두께", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", ClampMax = "30.0"))
	float SolidOutlineThickness = 7.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|아웃라인", DisplayName="반투명 외곽선 두께", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", ClampMax = "30.0"))
	float TransparentOutlineThickness = 7.0f;
};
