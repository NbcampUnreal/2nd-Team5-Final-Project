#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetPredefinedPatternLocation.generated.h"

UENUM(BlueprintType)
enum class EPredefinedPatternType : uint8
{
    Custom UMETA(DisplayName = "Custom (Manual Input)"),
    Line UMETA(DisplayName = "Line"),
    Square UMETA(DisplayName = "Square (Centered)"),
    Rectangle UMETA(DisplayName = "Rectangle (Centered)"),
    Circle UMETA(DisplayName = "Circle"),
    Grid UMETA(DisplayName = "Grid"),
    Star UMETA(DisplayName = "Star")
};

UCLASS(DisplayName="Get Predefined Pattern Location")
class STILLLOADING_API UBTTask_GetPredefinedPatternLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GetPredefinedPatternLocation();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Blackboard key to store/read the current pattern index."))
    FBlackboardKeySelector CurrentIndexKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Blackboard key to store the calculated absolute target location."))
    FBlackboardKeySelector TargetLocationKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Optional blackboard key to store if all pattern points have been visited."))
    FBlackboardKeySelector IsCompleteKey;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (ToolTip = "If true, AI's current location will be used as the pattern origin."))
    bool bUseActorLocationAsOrigin;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (EditCondition = "!bUseActorLocationAsOrigin", ToolTip = "Fixed origin for the pattern if bUseActorLocationAsOrigin is false."))
    FVector FixedPatternOrigin;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (EditCondition = "!bUseActorLocationAsOrigin", ToolTip = "Optional blackboard key to read the pattern origin. Overrides FixedPatternOrigin if set."))
    FBlackboardKeySelector OriginLocationKey;

    UPROPERTY(EditAnywhere, Category = "Pattern Behavior", meta = (ToolTip = "If true, the index will reset to 0 after completing the pattern, allowing infinite looping."))
    bool bLoopPattern;

    UPROPERTY(EditAnywhere, Category = "Pattern Behavior", meta = (ToolTip = "If true, the first point of the pattern IS the origin. If false, the first point is offset from the origin like other points."))
    bool bFirstPointIsOrigin;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition")
    EPredefinedPatternType PatternType;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Custom", meta = (EditCondition = "PatternType == EPredefinedPatternType::Custom", ToolTip = "Origin-relative locations for the custom pattern."))
    TArray<FVector> CustomRelativePatternLocations;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line"))
    float LineLength;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line", ClampMin = "1", ToolTip = "Minimum 1 point. If 1, it's at LineLength or Origin if bFirstPointIsOrigin."))
    int32 NumPointsOnLine;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line", ToolTip = "Direction of the line from the origin. Will be normalized."))
    FVector LineDirection;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Square", meta = (EditCondition = "PatternType == EPredefinedPatternType::Square"))
    float SquareSideLength;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Square", meta = (EditCondition = "PatternType == EPredefinedPatternType::Square", ClampMin = "4", ToolTip = "Number of points along the perimeter (e.g., 4 for corners, 8 for corners and midpoints). Will be distributed along perimeter."))
    int32 NumPointsOnSquarePerimeter;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle"))
    float RectangleWidth;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle"))
    float RectangleHeight;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle", ClampMin = "4", ToolTip = "Number of points along the perimeter. e.g., 4 for corners. More points will be distributed along edges."))
    int32 NumPointsOnRectanglePerimeter;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Circle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Circle"))
    float CircleRadius;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Circle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Circle", ClampMin = "1", ToolTip = "Minimum 1 point."))
    int32 NumPointsOnCircle;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ClampMin = "1"))
    int32 GridNumXPoints;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ClampMin = "1"))
    int32 GridNumYPoints;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid"))
    float GridSpacingX;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid"))
    float GridSpacingY;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ToolTip = "If true, centers the grid around the origin. Otherwise, origin is effectively the first point (bottom-left or equivalent)."))
    bool bCenterGrid;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Star", meta = (EditCondition = "PatternType == EPredefinedPatternType::Star", ClampMin = "3", ToolTip = "Number of points/peaks the star will have."))
    int32 NumStarPoints;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Star", meta = (EditCondition = "PatternType == EPredefinedPatternType::Star", ToolTip = "Outer radius of the star (distance from center to peaks)."))
    float StarOuterRadius;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Star", meta = (EditCondition = "PatternType == EPredefinedPatternType::Star", ToolTip = "Inner radius of the star (distance from center to valleys). Should be less than OuterRadius."))
    float StarInnerRadius;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Star", meta = (EditCondition = "PatternType == EPredefinedPatternType::Star", ToolTip = "Starting angle offset for the first peak of the star (in degrees)."))
    float StarStartAngleOffsetDegrees;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Star", meta = (EditCondition = "PatternType == EPredefinedPatternType::Star", ToolTip = "If true, includes points at the center of the star as the very first point."))
    bool bStarIncludeCenterPoint;

private:
    void GenerateRelativePatternLocations(TArray<FVector>& OutLocations) const;
    uint32 CalculateParameterHash() const;

    UPROPERTY(Transient)
    TArray<FVector> CachedRelativePatternLocations;

    uint32 LastGeneratedHash;
};