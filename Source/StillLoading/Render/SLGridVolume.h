#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLGridVolume.generated.h"

class USLGridNode;
class UCameraComponent;
class ASLGridVolume;
class UBoxComponent;

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
    
    UPROPERTY(VisibleAnywhere, Category = "GridNode")
    TObjectPtr<USLGridNode> UpGridNode;
    
    UPROPERTY(VisibleAnywhere, Category = "GridNode")
    TObjectPtr<USLGridNode> DownGridNode;
    
    UPROPERTY(VisibleAnywhere, Category = "GridNode")
    TObjectPtr<USLGridNode> LeftGridNode;
    
    UPROPERTY(VisibleAnywhere, Category = "GridNode")
    TObjectPtr<USLGridNode> RightGridNode;
    
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool ShouldTickIfViewportsOnly() const override{return true;}
    
#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void InitializeGridEdges();
    void InitializeTriggerVolume();
    void DrawGridBound() const;
    void DrawNodeConnection() const;
    
    UPROPERTY()
    TObjectPtr<USceneComponent> SceneComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridVolume", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridVolume", meta = (AllowPrivateAccess = "true"))
    bool bShowGridDebugLine = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridVolume", meta = (AllowPrivateAccess = "true"))
    float CameraPitch = -40.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridVolume", meta = (AllowPrivateAccess = "true"))
    float GridHeight = 1080.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridVolume", meta = (AllowPrivateAccess = "true"))
    float GridWidth = 1920.0f;

};