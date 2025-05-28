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

USTRUCT(BlueprintType)
struct FPixelArtSettings
{
	GENERATED_BODY()
	
	// 화면의 가로를 기준으로 그려지는 픽셀 수를 의미합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName= "픽셀화 크기", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1024"))
	int32 PixelSize = 512;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="흑백 명도", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float Brightness = 0.7f;
};

USTRUCT(BlueprintType)
struct FBaseOutlineSettings
{
	GENERATED_BODY()
    
	// SceneDepth 기준 - 최대 깊이 거리입니다. 
	// 이 값이 클수록 더 멀리 있는 물체까지 아웃라인이 적용됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", DisplayName = "최대 뎁스 거리", meta = (ClampMin = "500.0", ClampMax = "20000.0"))
	float MaxDepthRange = 10000.0f;
	
	// SceneDepth 기준 - 최소 깊이 거리입니다. 
	// 이 값이 MaxDepthRange에 가까울수록 아웃라인이 적용되는 거리 범위가 좁아지고, 멀리 있는 물체 위주로 강조됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", DisplayName = "최소 뎁스 거리", meta = (ClampMin = "100.0", ClampMax = "20000.0"))
	float MinDepthRange = 500.0f;

	// 최소 커널 사이즈입니다. 
	// 커널 사이즈는 엣지를 감지할 때 주변 픽셀을 얼마나 넓게 샘플링할지를 결정합니다.
	// 작을수록 섬세한 엣지를 감지할 수 있고, 클수록 뭉뚱그려진 아웃라인이 생성됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kernel", DisplayName = "최소 커널 크기", meta = (ClampMin = "1", ClampMax = "30"))
	int32 MinKernelSize = 1.0f;

	// 최대 커널 사이즈입니다. MinKernelSize와 함께 거리 기반으로 보간되며,
	// 멀리 있는 물체일수록 커널이 커지도록 설정해 아웃라인이 눈에 잘 띄게 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kernel", DisplayName = "최대 커널 크기", meta = (ClampMin = "1", ClampMax = "30"))
	int32 MaxKernelSize = 10.0f;
};


USTRUCT(BlueprintType)
struct FSolidOutlineSettings : public FBaseOutlineSettings
{
	GENERATED_BODY()
	
	// 노말 기반 엣지 감지 시 최소 임계값입니다.
	// 이 값보다 작으면 엣지로 인식하지 않으며, 노말 차이가 미미한 영역은 무시됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NormalEdge", DisplayName = "노말 엣지 최소 임계값", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NormalEdgeMinThreshold = 0.05f;

	// 노말 기반 엣지 감지 시 최대 임계값입니다.
	// 이 값보다 큰 경우 엣지 감지를 최대 강도로 처리합니다.
	// MinThreshold ~ MaxThreshold 범위는 보간 영역입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NormalEdge", DisplayName = "노말 엣지 최대 임계값", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NormalEdgeMaxThreshold = 0.1f;

	// 뎁스 기반 엣지 감지 시 최소 감지 기준값입니다.
	// 두 픽셀 간 깊이 차이가 이 값보다 작으면 엣지로 간주하지 않습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DepthEdge", DisplayName = "깊이 엣지 최소 임계값", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DepthEdgeMinThreshold = 0.1f;
	
	// 뎁스 기반 엣지 감지 시 최대 감지 기준값입니다.
	// 두 픽셀 간 깊이 차이가 이 값 이상이면 엣지로 강하게 처리됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DepthEdge", DisplayName = "깊이 엣지 최대 임계값", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DepthEdgeMaxThreshold = 0.5f;
};

USTRUCT(BlueprintType)
struct FSoftOutlineSettings : public FBaseOutlineSettings
{
	GENERATED_BODY()
    
	// 알파 기반 엣지 강도 임계값입니다.
	// 이 값보다 엣지 세기가 약한 픽셀은 렌더링되지 않으며,
	// 페이드된 아웃라인의 하한선을 설정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "엣지 최소 임계값", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EdgeAlphaThreshold  = 0.1f;
};

USTRUCT(BlueprintType)
struct FCelShaderSettings
{
	GENERATED_BODY()

	// 그레이드의 정도를 지정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "그레이드 계수", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ColorGrade = 0.2f;
	
	// 라이트에 영향을 받는 베이스 색상을 지정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "라이트 색상 그레이드")
	FLinearColor LightColor = FLinearColor(1.0f, 0.7f, 0.4f);

	// 그림자의 베이스 색상을 지정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "그림자 색상 그레이드")
	FLinearColor ShadowColor = FLinearColor(0.6f, 0.5f, 1.0f);
};

UCLASS(Blueprintable)
class STILLLOADING_API ASLRenderingManager : public AActor
{
	GENERATED_BODY()

public:
	ASLRenderingManager();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	void InitComponents();
	void UpdatePostProcessRenderingType();
	void UpdatePixelArtRenderingType();
	void UpdateSolidOutlineSettings();
	void UpdateSoftOutlineSettings();
	void UpdateCelShaderRenderingType();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLRendering", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialParameterCollection> MPCCelShader;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering", DisplayName= "픽셀 아트 렌더링", meta = (AllowPrivateAccess = "true"))
	FPixelArtSettings PixelArtSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|아웃라인", DisplayName="솔리드 아웃라인", meta = (AllowPrivateAccess = "true"))
	FSolidOutlineSettings SolidOutlineSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering|아웃라인", DisplayName="소프트 아웃라인", meta = (AllowPrivateAccess = "true"))
	FSoftOutlineSettings SoftOutlineSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLRendering", DisplayName="카툰 렌더링", meta = (AllowPrivateAccess = "true"))
	FCelShaderSettings CelShaderSettings;
};
