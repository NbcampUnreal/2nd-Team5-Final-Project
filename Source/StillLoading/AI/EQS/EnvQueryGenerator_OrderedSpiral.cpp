// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/EnvQueryGenerator_OrderedSpiral.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

UEnvQueryGenerator_OrderedSpiral::UEnvQueryGenerator_OrderedSpiral()
{
    CenterContext = UEnvQueryContext_Querier::StaticClass();
    ItemType = UEnvQueryItemType_Point::StaticClass();
}

void UEnvQueryGenerator_OrderedSpiral::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
    TArray<FVector> CenterLocations;
    QueryInstance.PrepareContext(CenterContext, CenterLocations);

    if (CenterLocations.Num() <= 0) return;

    // 안쪽부터 바깥쪽까지 순서대로 생성
    TArray<FVector> OrderedPoints;
    OrderedPoints.Reserve(PointCount);

    const FVector& Center = CenterLocations[0];
    
    // 순서대로 점 생성 (Progress가 작은 것부터)
    for (int32 i = 0; i < PointCount; ++i)
    {
        float Progress = (float)i / (float)(PointCount - 1);
        float Angle = Progress * NumberOfTurns * 2.0f * PI;
        float Radius = FMath::Lerp(MinRadius, MaxRadius, Progress);
        
        float X = Center.X + Radius * FMath::Cos(Angle);
        float Y = Center.Y + Radius * FMath::Sin(Angle);
        
        OrderedPoints.Add(FVector(X, Y, Center.Z));
    }
    
    // 순서가 보장된 상태로 추가
    QueryInstance.AddItemData<UEnvQueryItemType_Point>(OrderedPoints);
}
