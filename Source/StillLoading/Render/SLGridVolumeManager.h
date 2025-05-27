#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLGridVolume.h"
#include "SLGridVolumeManager.generated.h"

UCLASS()
class STILLLOADING_API ASLGridVolumeManager : public AActor
{
    GENERATED_BODY()
    
public:    
    ASLGridVolumeManager();
    
    // 함수 (public)
    UFUNCTION(BlueprintCallable, Category = "Grid")
    ASLGridVolume* GetGridVolume(int32 X, int32 Y) const;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    ASLGridVolume* GetGridVolumeAtWorldPosition(const FVector& WorldPosition) const;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    FVector GetWorldPositionFromGridCoordinate(int32 X, int32 Y) const;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    FIntPoint GetGridCoordinateFromWorldPosition(const FVector& WorldPosition) const;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void RegisterGridVolume(ASLGridVolume* Volume, int32 X, int32 Y);
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UnregisterGridVolume(ASLGridVolume* Volume);
    
    UFUNCTION(BlueprintCallable, Category = "Grid", CallInEditor)
    void RefreshAllGridVolumes();
    
    UFUNCTION(BlueprintCallable, Category = "Grid", CallInEditor)
    void GenerateGridVolumes();
    
    UFUNCTION(BlueprintCallable, Category = "Grid", CallInEditor)
    void ClearAllGridVolumes();
    
    // 게터 함수들
    float GetCellWidth() const;
    float GetCellHeight() const;
    int32 GetGridCountX() const { return GridCountX; }
    int32 GetGridCountY() const { return GridCountY; }
    float GetCameraPitch() const { return CameraFitch; }

protected:
    // 함수 (protected)
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // 변수 (protected)
    // 그리드 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "1"))
    int32 GridCountX = 5;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "1"))
    int32 GridCountY = 5;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "1.0"))
    float GridWidth = 800.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "1.0"))
    float GridHeight = 600.0f;
    
    // 카메라 설정 (기존 코드와 호환)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings", meta = (ClampMin = "0.0", ClampMax = "90.0"))
    float CameraFitch = 30.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
    float GroundHeight = 0.0f;
    
    // 에디터 표시 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowGlobalGrid = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bAutoGenerateVolumes = false; // 자동으로 그리드 볼륨 생성
    
    // 시각적 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FColor GridColor = FColor::White;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float LineThickness = 1.0f;

private:
    // 함수 (private)
    void InitializeGridArray();
    void DrawGlobalGrid() const;
    void UpdateExistingVolumes();
    bool HasGridSettingsChanged() const;
    void UpdateLastGridSettings();
    
    // 변수 (private)
    // 그리드 볼륨들을 관리하는 2D 배열
    TArray<TArray<TObjectPtr<ASLGridVolume>>> GridVolumes;
    
    // 마지막으로 업데이트된 그리드 설정 (변경 감지용)
    int32 LastGridCountX = 0;
    int32 LastGridCountY = 0;
    float LastGridWidth = 0.0f;
    float LastGridHeight = 0.0f;
};