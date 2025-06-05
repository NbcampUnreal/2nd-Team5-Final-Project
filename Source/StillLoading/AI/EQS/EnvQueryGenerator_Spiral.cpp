// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/EnvQueryGenerator_Spiral.h"

#include "NavigationSystem.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

UEnvQueryGenerator_Spiral::UEnvQueryGenerator_Spiral()
{
    // 기본 Context 설정 (Querier 주변에 생성)
    GenerateAroundContext = UEnvQueryContext_Querier::StaticClass();
    
    // 생성할 아이템 타입 설정
    ItemType = UEnvQueryItemType_Point::StaticClass();
}

void UEnvQueryGenerator_Spiral::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
    // Context로부터 중심점들을 가져옴
    TArray<FVector> ContextLocations;
    QueryInstance.PrepareContext(GenerateAroundContext, ContextLocations);

    if (ContextLocations.Num() <= 0)
    {
        return;
    }

    // 생성할 점들을 저장할 배열
    TArray<FNavLocation> NavPoints;
    NavPoints.Reserve(ContextLocations.Num() * PointCount);

    UWorld* World = QueryInstance.World;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

    // 각 Context 위치에서 나선형 생성
    for (const FVector& CenterLocation : ContextLocations)
    {
        for (int32 PointIndex = 0; PointIndex < PointCount; ++PointIndex)
        {
            // 0.0 ~ 1.0 사이의 진행도
            float Progress = (float)PointIndex / (float)(FMath::Max(1, PointCount - 1));
            
            // 나선형 점 계산
            FVector SpiralPoint = CalculateSpiralPoint(Progress, CenterLocation);
            
            // Navigation Mesh에 투영
            if (bProjectOnNavigation && NavSys)
            {
                FNavLocation NavLocation;
                if (NavSys->ProjectPointToNavigation(SpiralPoint, NavLocation))
                {
                    NavPoints.Add(NavLocation);
                }
            }
            else
            {
                // Navigation 투영 없이 직접 사용
                NavPoints.Add(FNavLocation(SpiralPoint));
            }
        }
    }

    // 점들을 쿼리 인스턴스에 추가
    if (NavPoints.Num() > 0)
    {
        // FNavLocation 배열을 FVector 배열로 변환
        TArray<FVector> PointLocations;
        PointLocations.Reserve(NavPoints.Num());
        
        for (const FNavLocation& NavLoc : NavPoints)
        {
            PointLocations.Add(NavLoc.Location);
        }
        
        // 아이템 데이터로 추가
        QueryInstance.AddItemData<UEnvQueryItemType_Point>(PointLocations);
    }
}

FVector UEnvQueryGenerator_Spiral::CalculateSpiralPoint(float Progress, const FVector& Center) const
{
    // 시작 각도 (라디안)
    float StartAngleRad = FMath::DegreesToRadians(StartAngleOffset);
    
    // 전체 회전 각도
    float TotalAngle = NumberOfTurns * 2.0f * PI;
    float CurrentAngle = StartAngleRad + (Progress * TotalAngle);
    
    // 시계 반대 방향인 경우 각도 반전
    if (!bClockwise)
    {
        CurrentAngle = -CurrentAngle;
    }

    float Radius = 0.0f;
    
    // 나선 타입에 따른 반지름 계산
    switch (SpiralType)
    {
        case ESpiralType::Archimedean:
            // 아르키메데스 나선: r = a + b*θ
            Radius = FMath::Lerp(MinRadius, MaxRadius, Progress);
            break;
            
        case ESpiralType::Logarithmic:
            // 로그 나선: r = a * e^(b*θ)
            if (MinRadius > 0)
            {
                float GrowthFactor = FMath::Loge(MaxRadius / MinRadius);
                Radius = MinRadius * FMath::Exp(GrowthFactor * Progress);
            }
            else
            {
                // MinRadius가 0인 경우 아르키메데스 나선으로 대체
                Radius = FMath::Lerp(MinRadius, MaxRadius, Progress);
            }
            break;
            
        case ESpiralType::Fermat:
            // 페르마 나선: r = a * √θ
            Radius = MinRadius + (MaxRadius - MinRadius) * FMath::Sqrt(Progress);
            break;
    }

    // 2D 좌표 계산
    float X = Center.X + Radius * FMath::Cos(CurrentAngle);
    float Y = Center.Y + Radius * FMath::Sin(CurrentAngle);
    
    // 높이 계산 (3D 나선인 경우)
    float Z = Center.Z + (HeightIncrement * Progress * NumberOfTurns);

    return FVector(X, Y, Z);
}

FText UEnvQueryGenerator_Spiral::GetDescriptionTitle() const
{
    return FText::FromString(FString::Printf(TEXT("Spiral: %d points"), PointCount));
}

FText UEnvQueryGenerator_Spiral::GetDescriptionDetails() const
{
    FString TypeString;
    switch (SpiralType)
    {
        case ESpiralType::Archimedean:
            TypeString = TEXT("Archimedean");
            break;
        case ESpiralType::Logarithmic:
            TypeString = TEXT("Logarithmic");
            break;
        case ESpiralType::Fermat:
            TypeString = TEXT("Fermat");
            break;
    }

    return FText::FromString(FString::Printf(
        TEXT("Type: %s\nTurns: %.1f\nRadius: %.0f-%.0f\nHeight Change: %.0f"),
        *TypeString,
        NumberOfTurns,
        MinRadius,
        MaxRadius,
        HeightIncrement
    ));
}