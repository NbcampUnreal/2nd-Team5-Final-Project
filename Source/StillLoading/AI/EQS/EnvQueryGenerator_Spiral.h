// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ProjectedPoints.h"
#include "EnvQueryGenerator_Spiral.generated.h"

UENUM()
enum class ESpiralType : uint8
{
	Archimedean     UMETA(DisplayName = "Archimedean (균등 간격)"),
	Logarithmic     UMETA(DisplayName = "Logarithmic (지수 증가)"),
	Fermat          UMETA(DisplayName = "Fermat (황금 나선)")
};
/**
 * 
 */
UCLASS()
class STILLLOADING_API UEnvQueryGenerator_Spiral : public UEnvQueryGenerator
{
    GENERATED_BODY()

public:
    UEnvQueryGenerator_Spiral();

    /** Context로부터 생성할 점들을 만듭니다 */
    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    /** 에디터에서 표시할 설명 텍스트 */
    virtual FText GetDescriptionTitle() const override;
    virtual FText GetDescriptionDetails() const override;
    
    /** 나선의 회전 수 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "0.5", ClampMax = "10.0"))
    float NumberOfTurns = 3.0f;

    /** 생성할 점의 개수 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "10", ClampMax = "200"))
    int32 PointCount = 50;

    /** 나선의 최소 반지름 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "0.0"))
    float MinRadius = 0.0f;

    /** 나선의 최대 반지름 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "100.0"))
    float MaxRadius = 1000.0f;

    /** 사용할 나선의 타입 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral")
    ESpiralType SpiralType = ESpiralType::Archimedean;

    /** 높이 변화 (3D 나선을 위해) */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "0.0"))
    float HeightIncrement = 0.0f;

    /** 시작 각도 오프셋 (도 단위) */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral", meta = (ClampMin = "0.0", ClampMax = "360.0"))
    float StartAngleOffset = 0.0f;

    /** 시계 방향으로 회전할지 여부 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral")
    bool bClockwise = true;

    /** Navigation에 투영할지 여부 */
    UPROPERTY(EditDefaultsOnly, Category = "Spiral")
    bool bProjectOnNavigation = true;

    /** 중심점을 결정할 Context */
    UPROPERTY(EditDefaultsOnly, Category = "Generator")
    TSubclassOf<UEnvQueryContext> GenerateAroundContext;
protected:
    /** 주어진 파라미터로 나선형 점을 계산 */
    FVector CalculateSpiralPoint(float Progress, const FVector& Center) const;
};
