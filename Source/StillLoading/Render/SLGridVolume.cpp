#include "SLGridVolume.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SLGridNode.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"

ASLGridVolume::ASLGridVolume()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(SceneComponent);
    
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SceneComponent);
    
    UpGridNode = CreateDefaultSubobject<USLGridNode>(TEXT("UpGridNode"));
    UpGridNode->SetupAttachment(SceneComponent);
    UpGridNode->SpawnPosition->SetupAttachment(UpGridNode);
    UpGridNode->TriggerVolume->SetupAttachment(UpGridNode);
    
    DownGridNode = CreateDefaultSubobject<USLGridNode>(TEXT("DownGridNode"));
    DownGridNode->SetupAttachment(SceneComponent);
    DownGridNode->SpawnPosition->SetupAttachment(DownGridNode);
    DownGridNode->TriggerVolume->SetupAttachment(DownGridNode);
    
    LeftGridNode = CreateDefaultSubobject<USLGridNode>(TEXT("LeftGridNode"));
    LeftGridNode->SetupAttachment(SceneComponent);
    LeftGridNode->SpawnPosition->SetupAttachment(LeftGridNode);
    LeftGridNode->TriggerVolume->SetupAttachment(LeftGridNode);
    
    RightGridNode = CreateDefaultSubobject<USLGridNode>(TEXT("RightGridNode"));
    RightGridNode->SetupAttachment(SceneComponent);
    RightGridNode->SpawnPosition->SetupAttachment(RightGridNode);
    RightGridNode->TriggerVolume->SetupAttachment(RightGridNode);
    
    InitializeGridEdges();
    InitializeTriggerVolume();
    
#if WITH_EDITOR
    PrimaryActorTick.bStartWithTickEnabled = true;
#endif
}

USLGridNode* ASLGridVolume::GetNodeByDirection(const EGridDirection Direction) const
{
    switch (Direction)
    {
        case EGridDirection::EGD_Up:
            return UpGridNode;
        case EGridDirection::EGD_Down:
            return DownGridNode;
        case EGridDirection::EGD_Left:
            return LeftGridNode;
        case EGridDirection::EGD_Right:
            return RightGridNode;
        default:
            return nullptr;
    }
}

void ASLGridVolume::BeginPlay()
{
    Super::BeginPlay();

}

void ASLGridVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
#if WITH_EDITOR
    DrawGridBound();
    DrawNodeConnection();
#endif
}

void ASLGridVolume::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}

void ASLGridVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    InitializeGridEdges();
    InitializeTriggerVolume();
}
#if WITH_EDITOR

void ASLGridVolume::InitializeGridEdges()
{
    const float HalfWidth = GetGridWidth() * 0.5f;
    const float HalfHeight = GetGridHeight() * 0.5f;
    
    UpGridNode->SpawnPosition->SetRelativeLocation(FVector(HalfHeight, 0.f, 0));
    DownGridNode->SpawnPosition->SetRelativeLocation(FVector(-HalfHeight, 0.f, 0));
    LeftGridNode->SpawnPosition->SetRelativeLocation(FVector(0.f, -HalfWidth, 0));
    RightGridNode->SpawnPosition->SetRelativeLocation(FVector(0.f, HalfWidth, 0));
}

void ASLGridVolume::InitializeTriggerVolume()
{
    const float HalfWidth = GetGridWidth() * 0.5f;
    const float HalfHeight = GetGridHeight() * 0.5f;

    constexpr float TriggerThickness = 50.0f;
    
    if (UpGridNode && UpGridNode->TriggerVolume)
    {
        UpGridNode->TriggerVolume->SetRelativeLocation(FVector(HalfHeight, 0.f, 0.f));
        UpGridNode->TriggerVolume->SetBoxExtent(FVector(TriggerThickness, HalfWidth, TriggerThickness));
        UpGridNode->TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
        UpGridNode->TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    if (DownGridNode && DownGridNode->TriggerVolume)
    {
        DownGridNode->TriggerVolume->SetRelativeLocation(FVector(-HalfHeight, 0.f, 0.f));
        DownGridNode->TriggerVolume->SetBoxExtent(FVector(TriggerThickness, HalfWidth, TriggerThickness));
        DownGridNode->TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
        DownGridNode->TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    if (LeftGridNode && LeftGridNode->TriggerVolume)
    {
        LeftGridNode->TriggerVolume->SetRelativeLocation(FVector(0.f, -HalfWidth, 0.f));
        LeftGridNode->TriggerVolume->SetBoxExtent(FVector(HalfHeight, TriggerThickness, TriggerThickness));
        LeftGridNode->TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
        LeftGridNode->TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    if (RightGridNode && RightGridNode->TriggerVolume)
    {
        RightGridNode->TriggerVolume->SetRelativeLocation(FVector(0.f, HalfWidth, 0.f));
        RightGridNode->TriggerVolume->SetBoxExtent(FVector(HalfHeight, TriggerThickness, TriggerThickness));
        RightGridNode->TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
        RightGridNode->TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}


#endif

void ASLGridVolume::DrawGridBound() const
{
      const UWorld* World = GetWorld();
    if (!World) return;

    const FVector Center = GetActorLocation();
    
    const float HalfWidth = GetGridWidth() * 0.5f;
    const float HalfHeight = GetGridHeight() * 0.5f;
    
    TArray<FVector> Corners = {
        Center + FVector(-HalfHeight, -HalfWidth, 0.f),
        Center + FVector(HalfHeight, -HalfWidth, 0.f),
        Center + FVector(HalfHeight, HalfWidth, 0.f),
        Center + FVector(-HalfHeight, HalfWidth, 0.f)
    };
    
    FColor BoundsColor = GridManager ? FColor::Cyan : FColor::Red;
    
    for (int32 i = 0; i < 4; ++i)
    {
        int32 NextIndex = (i + 1) % 4;
        DrawDebugLine(World, Corners[i], Corners[NextIndex], 
                     BoundsColor, false, -1.f, SDPG_Foreground, 5.0f);
    }
}

void ASLGridVolume::DrawNodeConnection() const
{
    const UWorld* World = GetWorld();
    if (!World) return;
    
    const float ArrowSize = 25.0f;
    const float ArrowThickness = 3.0f;
    const FColor ConnectedColor = FColor::Green;
    
    TArray<USLGridNode*> Nodes = { UpGridNode, DownGridNode, LeftGridNode, RightGridNode };
    
    for (const USLGridNode* Node : Nodes)
    {
        if (Node && Node->ConnectedVolume.IsValid())
        {
            const FVector ArrowStart = Node->SpawnPosition->GetComponentLocation();

            if (const ASLGridVolume* TargetVolume = Node->ConnectedVolume.Get())
            {
                const USLGridNode* TargetNode = TargetVolume->GetNodeByDirection(Node->NodeDirection);
                if (TargetNode && TargetNode->SpawnPosition)
                {
                    const FVector ArrowEnd = TargetNode->SpawnPosition->GetComponentLocation();
                    
                    DrawDebugDirectionalArrow(
                        World, 
                        ArrowStart, 
                        ArrowEnd, 
                        ArrowSize, 
                        ConnectedColor, 
                        false,
                        -1.f,
                        SDPG_Foreground, 
                        ArrowThickness
                    );
                }
            }
        }
    }
}

float ASLGridVolume::GetGridHeight() const
{
    const float PitchRadians = FMath::DegreesToRadians(CameraPitch);
    const float HeightScale = 1.f / FMath::Cos(PitchRadians);
    return GridHeight * HeightScale;
}

float ASLGridVolume::GetGridWidth() const
{
    return GridWidth;
}