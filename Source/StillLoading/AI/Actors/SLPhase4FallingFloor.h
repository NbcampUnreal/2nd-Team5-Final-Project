#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "SLPhase4FallingFloor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnFloorCollapseStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnFloorCollapseCompleted);

UCLASS()
class STILLLOADING_API ASLPhase4FallingFloor : public AActor
{
    GENERATED_BODY()

public:
    ASLPhase4FallingFloor();

    UFUNCTION(BlueprintCallable, Category = "Falling Floor")
    void StartFloorCollapse();

    UFUNCTION(BlueprintCallable, Category = "Falling Floor")
    void ResetFloor();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Setup")
    void RefreshMeshList();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Setup")
    void SetAllMeshesToFall();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Setup")
    void SetAllMeshesToSafe();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Setup")
    void TestStayPattern();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Setup")
    void ApplyDefaultPatterns();

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetMeshesToSafeByName(const TArray<FString>& SafePatterns);

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void SetMeshesToFallByName(const TArray<FString>& FallPatterns);

    UPROPERTY(BlueprintAssignable, Category = "Falling Floor")
    FSLOnFloorCollapseStarted OnFloorCollapseStarted;

    UPROPERTY(BlueprintAssignable, Category = "Falling Floor")
    FSLOnFloorCollapseCompleted OnFloorCollapseCompleted;

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    void CollectAllMeshComponents();
    void MakeMeshFall(UStaticMeshComponent* MeshComp);
    bool IsSafeMesh(UStaticMeshComponent* MeshComp) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TArray<TObjectPtr<UStaticMeshComponent>> AllMeshComponents;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe Meshes")
    TArray<TObjectPtr<UStaticMeshComponent>> SafeMeshComponents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float CollapseDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name Patterns")
    TArray<FString> DefaultSafePatterns;

private:
    void OnCollapseDelayFinished();

    FTimerHandle CollapseTimer;
};