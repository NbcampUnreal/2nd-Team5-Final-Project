#include "BTTask_GetPredefinedPatternLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Misc/Crc.h"


UBTTask_GetPredefinedPatternLocation::UBTTask_GetPredefinedPatternLocation()
{
    NodeName = "Get Predefined Pattern Location";

    CurrentIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, CurrentIndexKey));
    TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, TargetLocationKey));
    IsCompleteKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, IsCompleteKey));
    OriginLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, OriginLocationKey));

    // 기본값 설정
    bUseActorLocationAsOrigin = true;
    FixedPatternOrigin = FVector::ZeroVector;
    bLoopPattern = false;
    bFirstPointIsOrigin = false;
    PatternType = EPredefinedPatternType::Line;
    
    // Line 패턴 기본값
    LineLength = 500.0f;
    NumPointsOnLine = 5;
    LineDirection = FVector(1.0f, 0.0f, 0.0f);
    
    // Square 패턴 기본값
    SquareSideLength = 400.0f;
    NumPointsOnSquarePerimeter = 4;
    
    // Rectangle 패턴 기본값
    RectangleWidth = 600.0f;
    RectangleHeight = 300.0f;
    NumPointsOnRectanglePerimeter = 4;
    
    // Circle 패턴 기본값
    CircleRadius = 300.0f;
    NumPointsOnCircle = 8;
    
    // Grid 패턴 기본값
    GridNumXPoints = 3;
    GridNumYPoints = 3;
    GridSpacingX = 200.0f;
    GridSpacingY = 200.0f;
    bCenterGrid = true;
    
    // Star 패턴 기본값
    NumStarPoints = 5;
    StarOuterRadius = 500.0f;
    StarInnerRadius = 250.0f;
    StarStartAngleOffsetDegrees = 0.0f;
    bStarIncludeCenterPoint = false;
    
    // 캐시 관련 초기화
    LastGeneratedHash = 0;
}

void UBTTask_GetPredefinedPatternLocation::GenerateRelativePatternLocations(TArray<FVector>& OutLocations) const
{
    OutLocations.Reset();
    FVector FirstPointOffsetForOrigin = FVector::ZeroVector;
    
    switch (PatternType)
    {
        case EPredefinedPatternType::Line:
        {
            if (NumPointsOnLine < 1) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Line pattern requires at least 1 point"));
                return;
            }
            
            FVector Dir = LineDirection.GetSafeNormal();
            if (Dir.IsNearlyZero()) Dir = FVector(1,0,0);

            if (NumPointsOnLine == 1)
            {
                OutLocations.Add(bFirstPointIsOrigin ? FVector::ZeroVector : Dir * LineLength);
                return;
            }

            for (int32 i = 0; i < NumPointsOnLine; ++i)
            {
                float LerpAlpha = static_cast<float>(i) / (NumPointsOnLine - 1);
                OutLocations.Add(Dir * LerpAlpha * LineLength);
            }
            
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
        }
        
        case EPredefinedPatternType::Square:
        {
            if (NumPointsOnSquarePerimeter < 4) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Square pattern requires at least 4 points"));
                return;
            }

            float HalfSide = SquareSideLength / 2.0f;
            float Perimeter = 4.0f * SquareSideLength;
            
            if (Perimeter <= 0.f || NumPointsOnSquarePerimeter == 0) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Invalid square parameters"));
                return;
            }
            
            float DistBetweenPoints = Perimeter / static_cast<float>(NumPointsOnSquarePerimeter);

            for (int32 i = 0; i < NumPointsOnSquarePerimeter; ++i)
            {
                float TargetDist = i * DistBetweenPoints;
                
                if (TargetDist < SquareSideLength) // Bottom edge
                {
                    OutLocations.Add(FVector(-HalfSide + TargetDist, -HalfSide, 0));
                }
                else if (TargetDist < SquareSideLength * 2.0f) // Right edge
                {
                    OutLocations.Add(FVector(HalfSide, -HalfSide + (TargetDist - SquareSideLength), 0));
                }
                else if (TargetDist < SquareSideLength * 3.0f) // Top edge
                {
                    OutLocations.Add(FVector(HalfSide - (TargetDist - (SquareSideLength * 2.0f)), HalfSide, 0));
                }
                else // Left edge
                {
                    OutLocations.Add(FVector(-HalfSide, HalfSide - (TargetDist - (SquareSideLength * 3.0f)), 0));
                }
            }
            
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
        }
        
        case EPredefinedPatternType::Rectangle:
        {
            if (NumPointsOnRectanglePerimeter < 4) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Rectangle pattern requires at least 4 points"));
                return;
            }

            float HalfWidth = RectangleWidth / 2.0f;
            float HalfHeight = RectangleHeight / 2.0f;
            float Perimeter = 2.0f * (RectangleWidth + RectangleHeight);
            
            if (Perimeter <= 0.f || NumPointsOnRectanglePerimeter == 0) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Invalid rectangle parameters"));
                return;
            }

            float DistBetweenPoints = Perimeter / static_cast<float>(NumPointsOnRectanglePerimeter);

            for (int32 i = 0; i < NumPointsOnRectanglePerimeter; ++i)
            {
                float TargetDist = i * DistBetweenPoints;
                
                if (TargetDist < RectangleWidth) // Bottom edge
                {
                    OutLocations.Add(FVector(-HalfWidth + TargetDist, -HalfHeight, 0));
                }
                else if (TargetDist < RectangleWidth + RectangleHeight) // Right edge
                {
                    OutLocations.Add(FVector(HalfWidth, -HalfHeight + (TargetDist - RectangleWidth), 0));
                }
                else if (TargetDist < RectangleWidth * 2.0f + RectangleHeight) // Top edge
                {
                    OutLocations.Add(FVector(HalfWidth - (TargetDist - (RectangleWidth + RectangleHeight)), HalfHeight, 0));
                }
                else // Left edge
                {
                    OutLocations.Add(FVector(-HalfWidth, HalfHeight - (TargetDist - (RectangleWidth * 2.0f + RectangleHeight)), 0));
                }
            }
            
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
        }
        
        case EPredefinedPatternType::Circle:
        {
            if (NumPointsOnCircle < 1) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Circle pattern requires at least 1 point"));
                return;
            }
            
            float AngleStep = (2.0f * PI) / FMath::Max(1, NumPointsOnCircle);
            
            for (int32 i = 0; i < NumPointsOnCircle; ++i)
            {
                float Angle = i * AngleStep;
                OutLocations.Add(FVector(FMath::Cos(Angle) * CircleRadius, FMath::Sin(Angle) * CircleRadius, 0));
            }
            
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
        }
        
        case EPredefinedPatternType::Grid:
        {
            if (GridNumXPoints < 1 || GridNumYPoints < 1) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Grid pattern requires at least 1x1 points"));
                return;
            }

            float StartX = 0, StartY = 0;
            if (bCenterGrid)
            {
                StartX = - (static_cast<float>(GridNumXPoints - 1) * GridSpacingX) / 2.0f;
                StartY = - (static_cast<float>(GridNumYPoints - 1) * GridSpacingY) / 2.0f;
            }

            for (int32 y = 0; y < GridNumYPoints; ++y)
            {
                for (int32 x = 0; x < GridNumXPoints; ++x)
                {
                    FVector Point = FVector(StartX + x * GridSpacingX, StartY + y * GridSpacingY, 0);
                    OutLocations.Add(Point);
                }
            }
            
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
        }
        
        case EPredefinedPatternType::Star:
        {
            if (NumStarPoints < 3) 
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Star pattern requires at least 3 points"));
                return;
            }
            
            if (StarOuterRadius <= 0.f || StarInnerRadius < 0.f || StarInnerRadius >= StarOuterRadius)
            {
                UE_LOG(LogBehaviorTree, Warning, TEXT("GenerateRelativePatternLocations: Invalid star parameters. Ensure OuterRadius > 0, InnerRadius >= 0, and InnerRadius < OuterRadius."));
                return;
            }

            if (bStarIncludeCenterPoint)
            {
                OutLocations.Add(FVector::ZeroVector);
            }

            // 총 생성할 점의 개수는 첨점과 골짜기를 합한 것이므로 NumStarPoints * 2
            int32 TotalPointsForStarShape = NumStarPoints * 2;
            float AngleIncrement = (2.0f * PI) / static_cast<float>(TotalPointsForStarShape);
            float CurrentAngle = FMath::DegreesToRadians(StarStartAngleOffsetDegrees);

            int32 FirstStarPointIndex = bStarIncludeCenterPoint ? 1 : 0;

            for (int32 i = 0; i < TotalPointsForStarShape; ++i)
            {
                float RadiusToUse = (i % 2 == 0) ? StarOuterRadius : StarInnerRadius;
                float X = FMath::Cos(CurrentAngle) * RadiusToUse;
                float Y = FMath::Sin(CurrentAngle) * RadiusToUse;
                OutLocations.Add(FVector(X, Y, 0));
                CurrentAngle += AngleIncrement;
            }

            // bFirstPointIsOrigin 처리
            if (bFirstPointIsOrigin && OutLocations.Num() > FirstStarPointIndex)
            {
                FirstPointOffsetForOrigin = OutLocations[FirstStarPointIndex];
            }
            break;
        }
        
        case EPredefinedPatternType::Custom:
        default:
            OutLocations = CustomRelativePatternLocations;
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FirstPointOffsetForOrigin = OutLocations[0];
            }
            break;
    }

    // 첫 점을 원점으로 이동
    if (bFirstPointIsOrigin && !FirstPointOffsetForOrigin.IsNearlyZero() && OutLocations.Num() > 0)
    {
        for (FVector& Loc : OutLocations)
        {
            Loc -= FirstPointOffsetForOrigin;
        }
    }
}

uint32 UBTTask_GetPredefinedPatternLocation::CalculateParameterHash() const
{
    uint32 Hash = 0;
    Hash = FCrc::MemCrc32(&PatternType, sizeof(PatternType), Hash);
    Hash = FCrc::MemCrc32(&bFirstPointIsOrigin, sizeof(bFirstPointIsOrigin), Hash);

    switch (PatternType)
    {
        case EPredefinedPatternType::Custom:
            for (const FVector& Loc : CustomRelativePatternLocations)
            {
                Hash = FCrc::MemCrc32(&Loc, sizeof(FVector), Hash);
            }
            break;
        case EPredefinedPatternType::Line:
            Hash = FCrc::MemCrc32(&LineLength, sizeof(LineLength), Hash);
            Hash = FCrc::MemCrc32(&NumPointsOnLine, sizeof(NumPointsOnLine), Hash);
            Hash = FCrc::MemCrc32(&LineDirection, sizeof(LineDirection), Hash);
            break;
        case EPredefinedPatternType::Square:
            Hash = FCrc::MemCrc32(&SquareSideLength, sizeof(SquareSideLength), Hash);
            Hash = FCrc::MemCrc32(&NumPointsOnSquarePerimeter, sizeof(NumPointsOnSquarePerimeter), Hash);
            break;
        case EPredefinedPatternType::Rectangle:
            Hash = FCrc::MemCrc32(&RectangleWidth, sizeof(RectangleWidth), Hash);
            Hash = FCrc::MemCrc32(&RectangleHeight, sizeof(RectangleHeight), Hash);
            Hash = FCrc::MemCrc32(&NumPointsOnRectanglePerimeter, sizeof(NumPointsOnRectanglePerimeter), Hash);
            break;
        case EPredefinedPatternType::Circle:
            Hash = FCrc::MemCrc32(&CircleRadius, sizeof(CircleRadius), Hash);
            Hash = FCrc::MemCrc32(&NumPointsOnCircle, sizeof(NumPointsOnCircle), Hash);
            break;
        case EPredefinedPatternType::Grid:
            Hash = FCrc::MemCrc32(&GridNumXPoints, sizeof(GridNumXPoints), Hash);
            Hash = FCrc::MemCrc32(&GridNumYPoints, sizeof(GridNumYPoints), Hash);
            Hash = FCrc::MemCrc32(&GridSpacingX, sizeof(GridSpacingX), Hash);
            Hash = FCrc::MemCrc32(&GridSpacingY, sizeof(GridSpacingY), Hash);
            Hash = FCrc::MemCrc32(&bCenterGrid, sizeof(bCenterGrid), Hash);
            break;
        case EPredefinedPatternType::Star:
            Hash = FCrc::MemCrc32(&NumStarPoints, sizeof(NumStarPoints), Hash);
            Hash = FCrc::MemCrc32(&StarOuterRadius, sizeof(StarOuterRadius), Hash);
            Hash = FCrc::MemCrc32(&StarInnerRadius, sizeof(StarInnerRadius), Hash);
            Hash = FCrc::MemCrc32(&StarStartAngleOffsetDegrees, sizeof(StarStartAngleOffsetDegrees), Hash);
            Hash = FCrc::MemCrc32(&bStarIncludeCenterPoint, sizeof(bStarIncludeCenterPoint), Hash);
            break;
    }
    return Hash;
}

void UBTTask_GetPredefinedPatternLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    uint32 CurrentHash = CalculateParameterHash();
    if (CurrentHash != LastGeneratedHash || CachedRelativePatternLocations.Num() == 0)
    {
        GenerateRelativePatternLocations(CachedRelativePatternLocations);
        LastGeneratedHash = CurrentHash;
    }
}

#if WITH_EDITOR
void UBTTask_GetPredefinedPatternLocation::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (PropertyChangedEvent.Property != nullptr)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        
        // 모든 패턴 관련 속성을 체크
        if (PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, PatternType) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, CustomRelativePatternLocations) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, LineLength) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, NumPointsOnLine) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, LineDirection) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, SquareSideLength) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, NumPointsOnSquarePerimeter) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, RectangleWidth) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, RectangleHeight) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, NumPointsOnRectanglePerimeter) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, CircleRadius) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, NumPointsOnCircle) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, GridNumXPoints) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, GridNumYPoints) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, GridSpacingX) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, GridSpacingY) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, bCenterGrid) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, NumStarPoints) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, StarOuterRadius) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, StarInnerRadius) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, StarStartAngleOffsetDegrees) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, bStarIncludeCenterPoint) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, bFirstPointIsOrigin))
        {
            uint32 CurrentHash = CalculateParameterHash();
            if (CurrentHash != LastGeneratedHash || CachedRelativePatternLocations.Num() == 0)
            {
                GenerateRelativePatternLocations(CachedRelativePatternLocations);
                LastGeneratedHash = CurrentHash;
            }
        }
    }
}
#endif

EBTNodeResult::Type UBTTask_GetPredefinedPatternLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogBehaviorTree, Error, TEXT("UBTTask_GetPredefinedPatternLocation: BlackboardComponent is null"));
        return EBTNodeResult::Failed;
    }

    // 캐시 확인 및 재생성
    uint32 CurrentHash = CalculateParameterHash();
    if (CurrentHash != LastGeneratedHash || CachedRelativePatternLocations.Num() == 0)
    {
        GenerateRelativePatternLocations(CachedRelativePatternLocations);
        LastGeneratedHash = CurrentHash;
    }

    if (CachedRelativePatternLocations.Num() == 0)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("UBTTask_GetPredefinedPatternLocation: No pattern locations generated"));
        return EBTNodeResult::Failed;
    }

    // 현재 인덱스 가져오기
    int32 CurrentIndex = 0;
    if (CurrentIndexKey.SelectedKeyName != NAME_None)
    {
        CurrentIndex = BlackboardComp->GetValueAsInt(CurrentIndexKey.SelectedKeyName);
    }

    // 패턴 완료 확인
    bool bPatternComplete = CurrentIndex >= CachedRelativePatternLocations.Num();
    
    if (bPatternComplete)
    {
        if (bLoopPattern)
        {
            CurrentIndex = 0;
            if (CurrentIndexKey.SelectedKeyName != NAME_None)
            {
                BlackboardComp->SetValueAsInt(CurrentIndexKey.SelectedKeyName, 0);
            }
            if (IsCompleteKey.SelectedKeyName != NAME_None)
            {
                BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, false);
            }
        }
        else
        {
            if (IsCompleteKey.SelectedKeyName != NAME_None)
            {
                BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, true);
            }
            UE_LOG(LogBehaviorTree, Verbose, TEXT("UBTTask_GetPredefinedPatternLocation: Pattern completed (non-looping)"));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        // 마지막 포인트 확인
        bool bIsLastPoint = (CurrentIndex == CachedRelativePatternLocations.Num() - 1);
        if (IsCompleteKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, !bLoopPattern && bIsLastPoint);
        }
    }

    // 패턴 원점 결정
    FVector PatternOrigin = FVector::ZeroVector;
    if (bUseActorLocationAsOrigin)
    {
        AAIController* AIController = OwnerComp.GetAIOwner();
        APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
        if (!Pawn)
        {
            UE_LOG(LogBehaviorTree, Error, TEXT("UBTTask_GetPredefinedPatternLocation: Pawn is null"));
            return EBTNodeResult::Failed;
        }
        PatternOrigin = Pawn->GetActorLocation();
    }
    else
    {
        if (OriginLocationKey.SelectedKeyName != NAME_None)
        {
            PatternOrigin = BlackboardComp->GetValueAsVector(OriginLocationKey.SelectedKeyName);
        }
        else
        {
            PatternOrigin = FixedPatternOrigin;
        }
    }

    // 절대 위치 계산
    FVector AbsoluteTargetLocation = PatternOrigin + CachedRelativePatternLocations[CurrentIndex];

    // 타겟 위치 설정
    if (TargetLocationKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, AbsoluteTargetLocation);
    }
    else
    {
        UE_LOG(LogBehaviorTree, Error, TEXT("UBTTask_GetPredefinedPatternLocation: TargetLocationKey is not set"));
        return EBTNodeResult::Failed;
    }
    
    // 다음 인덱스로 업데이트
    if (CurrentIndexKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsInt(CurrentIndexKey.SelectedKeyName, CurrentIndex + 1);
    }
    
    return EBTNodeResult::Succeeded;
}

FString UBTTask_GetPredefinedPatternLocation::GetStaticDescription() const
{
    FString Description = FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(PatternType));
    if (bFirstPointIsOrigin) Description += TEXT(" (Origin is 1st Pt)");

    switch (PatternType)
    {
        case EPredefinedPatternType::Line:
            Description += FString::Printf(TEXT("\nLen:%.0f, Pts:%d"), LineLength, NumPointsOnLine);
            break;
        case EPredefinedPatternType::Square:
            Description += FString::Printf(TEXT("\nSide:%.0f, Pts:%d"), SquareSideLength, NumPointsOnSquarePerimeter);
            break;
        case EPredefinedPatternType::Rectangle:
            Description += FString::Printf(TEXT("\n%.0fx%.0f, Pts:%d"), RectangleWidth, RectangleHeight, NumPointsOnRectanglePerimeter);
            break;
        case EPredefinedPatternType::Circle:
            Description += FString::Printf(TEXT("\nRad:%.0f, Pts:%d"), CircleRadius, NumPointsOnCircle);
            break;
        case EPredefinedPatternType::Grid:
            Description += FString::Printf(TEXT("\n%dx%d Grid, SpX:%.0f SpY:%.0f"), GridNumXPoints, GridNumYPoints, GridSpacingX, GridSpacingY);
            if (bCenterGrid) Description += TEXT(" (Centered)");
            break;
        case EPredefinedPatternType::Star:
            Description += FString::Printf(TEXT("\nPts:%d, OR:%.0f, IR:%.0f"), NumStarPoints, StarOuterRadius, StarInnerRadius);
            if (bStarIncludeCenterPoint) Description += TEXT(" (w/Center)");
            break;
        case EPredefinedPatternType::Custom:
            Description += FString::Printf(TEXT("\nPts: %d (Custom)"), CachedRelativePatternLocations.Num());
            break;
    }
    
    if (bLoopPattern) Description += TEXT(" (Looping)");

    if (TargetLocationKey.SelectedKeyName != NAME_None)
    {
        Description += FString::Printf(TEXT("\nTarget: %s"), *TargetLocationKey.SelectedKeyName.ToString());
    }
    if (CurrentIndexKey.SelectedKeyName != NAME_None)
    {
        Description += FString::Printf(TEXT("\nIndex: %s"), *CurrentIndexKey.SelectedKeyName.ToString());
    }
    
    return Description;
}