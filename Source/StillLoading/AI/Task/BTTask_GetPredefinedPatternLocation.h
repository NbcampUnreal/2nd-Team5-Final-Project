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
    Grid UMETA(DisplayName = "Grid")
};

UCLASS(DisplayName="Get Predefined Pattern Location")
class STILLLOADING_API UBTTask_GetPredefinedPatternLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GetPredefinedPatternLocation();

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Blackboard key to store/read the current pattern index."))
    FBlackboardKeySelector CurrentIndexKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Blackboard key to store the calculated absolute target location."))
    FBlackboardKeySelector TargetLocationKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ToolTip = "Optional blackboard key to store if all pattern points have been visited."))
    FBlackboardKeySelector IsCompleteKey;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (ToolTip = "If true, AI's current location will be used as the pattern origin."))
    bool bUseActorLocationAsOrigin = true;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (EditCondition = "!bUseActorLocationAsOrigin", ToolTip = "Fixed origin for the pattern if bUseActorLocationAsOrigin is false."))
    FVector FixedPatternOrigin = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Origin", meta = (EditCondition = "!bUseActorLocationAsOrigin", ToolTip = "Optional blackboard key to read the pattern origin. Overrides FixedPatternOrigin if set."))
    FBlackboardKeySelector OriginLocationKey;

    UPROPERTY(EditAnywhere, Category = "Pattern Behavior", meta = (ToolTip = "If true, the index will reset to 0 after completing the pattern, allowing infinite looping."))
    bool bLoopPattern = false;

    UPROPERTY(EditAnywhere, Category = "Pattern Behavior", meta = (ToolTip = "If true, the first point of the pattern IS the origin. If false, the first point is offset from the origin like other points."))
    bool bFirstPointIsOrigin = false;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition")
    EPredefinedPatternType PatternType = EPredefinedPatternType::Line;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Custom", meta = (EditCondition = "PatternType == EPredefinedPatternType::Custom", ToolTip = "Origin-relative locations for the custom pattern."))
    TArray<FVector> CustomRelativePatternLocations;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line"))
    float LineLength = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line", ClampMin = "1", ToolTip = "Minimum 1 point. If 1, it's at LineLength or Origin if bFirstPointIsOrigin."))
    int32 NumPointsOnLine = 5;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Line", meta = (EditCondition = "PatternType == EPredefinedPatternType::Line", ToolTip = "Direction of the line from the origin. Will be normalized."))
    FVector LineDirection = FVector(1.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Square", meta = (EditCondition = "PatternType == EPredefinedPatternType::Square"))
    float SquareSideLength = 400.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Square", meta = (EditCondition = "PatternType == EPredefinedPatternType::Square", ClampMin = "4", ToolTip = "Number of points along the perimeter (e.g., 4 for corners, 8 for corners and midpoints). Will be distributed along perimeter."))
    int32 NumPointsOnSquarePerimeter = 4;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle"))
    float RectangleWidth = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle"))
    float RectangleHeight = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Rectangle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Rectangle", ClampMin = "4", ToolTip = "Number of points along the perimeter. e.g., 4 for corners. More points will be distributed along edges."))
    int32 NumPointsOnRectanglePerimeter = 4;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Circle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Circle"))
    float CircleRadius = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Circle", meta = (EditCondition = "PatternType == EPredefinedPatternType::Circle", ClampMin = "1", ToolTip = "Minimum 1 point."))
    int32 NumPointsOnCircle = 8;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ClampMin = "1"))
    int32 GridNumXPoints = 3;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ClampMin = "1"))
    int32 GridNumYPoints = 3;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid"))
    float GridSpacingX = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid"))
    float GridSpacingY = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Pattern Definition|Grid", meta = (EditCondition = "PatternType == EPredefinedPatternType::Grid", ToolTip = "If true, centers the grid around the origin. Otherwise, origin is effectively the first point (bottom-left or equivalent)."))
    bool bCenterGrid = true;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void GenerateRelativePatternLocations(TArray<FVector>& OutLocations) const;

    UPROPERTY(Transient)
    TArray<FVector> CachedRelativePatternLocations;

    uint32 LastGeneratedHash = 0;
    uint32 CalculateParameterHash() const;
};