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
}

void UBTTask_GetPredefinedPatternLocation::GenerateRelativePatternLocations(TArray<FVector>& OutLocations) const
{
    OutLocations.Reset();

    switch (PatternType)
    {
        case EPredefinedPatternType::Line:
        {
            if (NumPointsOnLine < 1) return;
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
                if (bFirstPointIsOrigin && i == 0)
                {
                    OutLocations.Add(FVector::ZeroVector);
                }
                else
                {
                    OutLocations.Add(Dir * LerpAlpha * LineLength);
                }
            }
            break;
        }
        case EPredefinedPatternType::Square:
        {
            if (NumPointsOnSquarePerimeter < 4) return;

            float HalfSide = SquareSideLength / 2.0f;
            TArray<FVector> Corners;
            Corners.Add(FVector(-HalfSide, -HalfSide, 0));
            Corners.Add(FVector( HalfSide, -HalfSide, 0));
            Corners.Add(FVector( HalfSide,  HalfSide, 0));
            Corners.Add(FVector(-HalfSide,  HalfSide, 0));
            
            float Perimeter = 4.0f * SquareSideLength;
            if (Perimeter <= 0.f || NumPointsOnSquarePerimeter == 0) return;
            float DistBetweenPoints = Perimeter / static_cast<float>(NumPointsOnSquarePerimeter);

            for (int32 i = 0; i < NumPointsOnSquarePerimeter; ++i)
            {
                float TargetDist = i * DistBetweenPoints;
                if (TargetDist <= SquareSideLength) 
                {
                    OutLocations.Add(FVector(-HalfSide + TargetDist, -HalfSide, 0));
                }
                else if (TargetDist <= SquareSideLength * 2.0f) 
                {
                    OutLocations.Add(FVector(HalfSide, -HalfSide + (TargetDist - SquareSideLength), 0));
                }
                else if (TargetDist <= SquareSideLength * 3.0f) 
                {
                    OutLocations.Add(FVector(HalfSide - (TargetDist - (SquareSideLength * 2.0f)), HalfSide, 0));
                }
                else 
                {
                     OutLocations.Add(FVector(-HalfSide, HalfSide - (TargetDist - (SquareSideLength * 3.0f)), 0));
                }
            }
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FVector FirstOffset = OutLocations[0];
                for(FVector& Loc : OutLocations) { Loc -= FirstOffset; }
            }
            break;
        }
        case EPredefinedPatternType::Rectangle:
        {
            if (NumPointsOnRectanglePerimeter < 4) return;

            float HalfWidth = RectangleWidth / 2.0f;
            float HalfHeight = RectangleHeight / 2.0f;
            
            float Perimeter = 2.0f * (RectangleWidth + RectangleHeight);
            if (Perimeter <= 0.f || NumPointsOnRectanglePerimeter == 0) return;

            float DistBetweenPoints = Perimeter / static_cast<float>(NumPointsOnRectanglePerimeter);

            for (int32 i = 0; i < NumPointsOnRectanglePerimeter; ++i)
            {
                float TargetDist = i * DistBetweenPoints;
                if (TargetDist <= RectangleWidth) 
                {
                    OutLocations.Add(FVector(-HalfWidth + TargetDist, -HalfHeight, 0));
                }
                else if (TargetDist <= RectangleWidth + RectangleHeight) 
                {
                    OutLocations.Add(FVector(HalfWidth, -HalfHeight + (TargetDist - RectangleWidth), 0));
                }
                else if (TargetDist <= RectangleWidth * 2.0f + RectangleHeight) 
                {
                    OutLocations.Add(FVector(HalfWidth - (TargetDist - (RectangleWidth + RectangleHeight)), HalfHeight, 0));
                }
                else 
                {
                     OutLocations.Add(FVector(-HalfWidth, HalfHeight - (TargetDist - (RectangleWidth * 2.0f + RectangleHeight)), 0));
                }
            }
            if (bFirstPointIsOrigin && OutLocations.Num() > 0)
            {
                FVector FirstOffset = OutLocations[0];
                for(FVector& Loc : OutLocations) { Loc -= FirstOffset; }
            }
            break;
        }
        case EPredefinedPatternType::Circle:
        {
            if (NumPointsOnCircle < 1) return;
            float AngleStep = (2.0f * PI) / FMath::Max(1, NumPointsOnCircle);
            for (int32 i = 0; i < NumPointsOnCircle; ++i)
            {
                float Angle = i * AngleStep;
                if (bFirstPointIsOrigin && i == 0)
                {
                     OutLocations.Add(FVector::ZeroVector);
                }
                else
                {
                    OutLocations.Add(FVector(FMath::Cos(Angle) * CircleRadius, FMath::Sin(Angle) * CircleRadius, 0));
                }
            }
            break;
        }
        case EPredefinedPatternType::Grid:
        {
            if (GridNumXPoints < 1 || GridNumYPoints < 1) return;

            float StartX = 0, StartY = 0;
            if (bCenterGrid)
            {
                StartX = - (static_cast<float>(GridNumXPoints - 1) * GridSpacingX) / 2.0f;
                StartY = - (static_cast<float>(GridNumYPoints - 1) * GridSpacingY) / 2.0f;
            }

            FVector FirstPointOffset = FVector::ZeroVector;
            if (bFirstPointIsOrigin)
            {
                FirstPointOffset = FVector(StartX, StartY, 0);
            }

            for (int32 y = 0; y < GridNumYPoints; ++y)
            {
                for (int32 x = 0; x < GridNumXPoints; ++x)
                {
                    FVector Point = FVector(StartX + x * GridSpacingX, StartY + y * GridSpacingY, 0);
                    OutLocations.Add(Point - FirstPointOffset);
                }
            }
            break;
        }
        case EPredefinedPatternType::Custom:
        default:
            if (bFirstPointIsOrigin && CustomRelativePatternLocations.Num() > 0)
            {
                FVector FirstOffset = CustomRelativePatternLocations[0];
                OutLocations.Add(FVector::ZeroVector); 
                for(int i = 1; i < CustomRelativePatternLocations.Num(); ++i) 
                {
                    OutLocations.Add(CustomRelativePatternLocations[i] - FirstOffset);
                }
            }
            else
            {
                 OutLocations = CustomRelativePatternLocations;
            }
            break;
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
            PropertyName == GET_MEMBER_NAME_CHECKED(UBTTask_GetPredefinedPatternLocation, bFirstPointIsOrigin))
        {
            uint32 CurrentHash = CalculateParameterHash();
            if (CurrentHash != LastGeneratedHash || CachedRelativePatternLocations.Num() == 0 )
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
        return EBTNodeResult::Failed;
    }

    if (CachedRelativePatternLocations.Num() == 0 && PatternType != EPredefinedPatternType::Custom)
    {
        uint32 CurrentHash = CalculateParameterHash();
        GenerateRelativePatternLocations(CachedRelativePatternLocations);
        LastGeneratedHash = CurrentHash;
        if (CachedRelativePatternLocations.Num() == 0 && PatternType != EPredefinedPatternType::Custom)
        {
            return EBTNodeResult::Failed;
        }
    }
    else if (PatternType == EPredefinedPatternType::Custom && CustomRelativePatternLocations.Num() == 0 && !bFirstPointIsOrigin)
    {
         return EBTNodeResult::Failed;
    }
     else if (PatternType == EPredefinedPatternType::Custom && CustomRelativePatternLocations.Num() == 0 && bFirstPointIsOrigin)
    {
        if(CachedRelativePatternLocations.Num() != 1 || CachedRelativePatternLocations[0] != FVector::ZeroVector)
        {
            GenerateRelativePatternLocations(CachedRelativePatternLocations);
        }
    }
    
    const TArray<FVector>& ActivePatternLocations = CachedRelativePatternLocations;

    if (ActivePatternLocations.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    int32 CurrentIndex = 0;
    if (CurrentIndexKey.SelectedKeyName != NAME_None)
    {
       CurrentIndex = BlackboardComp->GetValueAsInt(CurrentIndexKey.SelectedKeyName);
    }

    if (CurrentIndex >= ActivePatternLocations.Num())
    {
        if (bLoopPattern)
        {
            CurrentIndex = 0;
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
            return EBTNodeResult::Failed;
        }
    }
    else 
    {
        if (IsCompleteKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(IsCompleteKey.SelectedKeyName, false);
        }
    }

    FVector PatternOrigin = FVector::ZeroVector;
    if (bUseActorLocationAsOrigin)
    {
        AAIController* AIController = OwnerComp.GetAIOwner();
        APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
        if (!Pawn)
        {
            return EBTNodeResult::Failed;
        }
        PatternOrigin = Pawn->GetActorLocation();
    }
    else
    {
        if (OriginLocationKey.SelectedKeyName != NAME_None && OriginLocationKey.GetSelectedKeyID() != FBlackboard::InvalidKey && BlackboardComp->GetValueAsObject(OriginLocationKey.SelectedKeyName) != nullptr)
        {
            PatternOrigin = BlackboardComp->GetValueAsVector(OriginLocationKey.SelectedKeyName);
        }
        else
        {
            PatternOrigin = FixedPatternOrigin;
        }
    }

    if (CurrentIndex < 0 || CurrentIndex >= ActivePatternLocations.Num())
    {
        return EBTNodeResult::Failed;
    }
    FVector AbsoluteTargetLocation = PatternOrigin + ActivePatternLocations[CurrentIndex];

    if (TargetLocationKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, AbsoluteTargetLocation);
    }
    else
    {
        return EBTNodeResult::Failed; 
    }
    
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