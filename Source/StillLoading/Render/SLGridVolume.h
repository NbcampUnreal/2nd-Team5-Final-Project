#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLGridVolume.generated.h"

class USLGridNode;
class UCameraComponent;
class ASLGridVolume;
class UBoxComponent;
class ASLGridVolumeManager;

UENUM(BlueprintType)
enum class EGridDirection : uint8
{
    EGD_Up,
    EGD_Down,
    EGD_Left,
    EGD_Right,
};

UCLASS()
class STILLLOADING_API ASLGridVolume : public AActor
{
    GENERATED_BODY()
    
public:
    ASLGridVolume();

    USLGridNode* GetNodeByDirection(EGridDirection Direction) const;
    float GetGridHeight() const;
    float GetGridWidth() const;
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TObjectPtr<USLGridNode> UpGridNode;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TObjectPtr<USLGridNode> DownGridNode;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TObjectPtr<USLGridNode> LeftGridNode;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TObjectPtr<USLGridNode> RightGridNode;
    
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool ShouldTickIfViewportsOnly() const override{return true;}
    
#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    FIntPoint GridCoordinate;

private:
    void InitializeGridEdges();
    void InitializeTriggerVolume();
    void DrawGridBound() const;
    void DrawNodeConnection() const;
    
    UPROPERTY()
    TObjectPtr<ASLGridVolumeManager> GridManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    float CameraPitch = -40.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    float GridHeight = 1080.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    float GridWidth = 1920.0f;

};