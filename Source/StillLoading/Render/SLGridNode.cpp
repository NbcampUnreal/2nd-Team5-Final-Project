// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGridNode.h"

#include "SLGridVolume.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


USLGridNode::USLGridNode()
{
	PrimaryComponentTick.bCanEverTick = false;
    
	FString CompName = GetName() + "_TriggerVolume";
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName(*CompName));
    
	CompName = GetName() + "_SpawnPosition";
	SpawnPosition = CreateDefaultSubobject<UBillboardComponent>(FName(*CompName));
}

void USLGridNode::UpdateTriggerVolume()
{
    if (!TriggerVolume) return;

    TriggerVolume->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    TriggerVolume->SetCollisionResponseToAllChannels(ECR_Block);
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    if (ConnectedVolume.IsValid())
    {
        TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

void USLGridNode::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &USLGridNode::OnTriggeredNode);

    UpdateTriggerVolume();
}

#if WITH_EDITOR
void USLGridNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    UpdateTriggerVolume();
}
#endif

void USLGridNode::OnTriggeredNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                  bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character) return;

    UCapsuleComponent* CharacterCapsule = Character->GetCapsuleComponent();
    if (!CharacterCapsule) return;
    
    APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
    if (!PlayerController) return;

    ASLGridVolume* TargetVolume = ConnectedVolume.Get();
    if (!TargetVolume) return;

    USLGridNode* TargetNode = TargetVolume->GetNodeByDirection(NodeDirection);
    if (!TargetNode) return;
    
    FVector TargetLocation;
    
    float TriggerThickness = 0.f;
    
    if (TargetNode->TriggerVolume)
    {
        FVector TriggerExtent = TargetNode->TriggerVolume->GetScaledBoxExtent();
        switch (NodeDirection)
        {
        case EGridDirection::EGD_Up:
        case EGridDirection::EGD_Down:
            TriggerThickness = TriggerExtent.X * 2.0f;
            break;
        case EGridDirection::EGD_Left:
        case EGridDirection::EGD_Right:
            TriggerThickness = TriggerExtent.Y * 2.0f;
            break;
        }
    }
    
    float CapsuleRadius = CharacterCapsule->GetScaledCapsuleRadius();
    float CapsuleHalfHeight = CharacterCapsule->GetScaledCapsuleHalfHeight();
    
    float SafeDistance = TriggerThickness + CapsuleRadius + 10.0f;
    
    if (NodeDirection == EGridDirection::EGD_Up || NodeDirection == EGridDirection::EGD_Down)
    {
        SafeDistance = FMath::Max(SafeDistance, CapsuleHalfHeight + 10.0f);
    }
    
     if (bUseSpawnPosition && TargetNode->SpawnPosition)
    {
        FVector TriggerLocation = TargetNode->TriggerVolume->GetComponentLocation();
        FVector SpawnLocation = TargetNode->SpawnPosition->GetComponentLocation();
        
        FVector2D TriggerLocation2D(TriggerLocation.X, TriggerLocation.Y);
        FVector2D SpawnLocation2D(SpawnLocation.X, SpawnLocation.Y);
        FVector2D DirectionVector2D;
        
        switch (NodeDirection)
        {
        case EGridDirection::EGD_Up:
            DirectionVector2D = FVector2D(1.0f, 0.0f);
            break;
        case EGridDirection::EGD_Down:
            DirectionVector2D = FVector2D(-1.0f, 0.0f);
            break;
        case EGridDirection::EGD_Left:
            DirectionVector2D = FVector2D(0.0f, -1.0f);
            break;
        case EGridDirection::EGD_Right:
            DirectionVector2D = FVector2D(0.0f, 1.0f);
            break;
        }
        
        FVector EdgePosition;
        const FVector TargetVolumeLocation = TargetVolume->GetActorLocation();
        const float TargetGridWidth = TargetVolume->GetGridWidth();
        const float TargetGridHeight = TargetVolume->GetGridHeight();
        const float TargetHalfWidth = TargetGridWidth * 0.5f;
        const float TargetHalfHeight = TargetGridHeight * 0.5f;
        
        switch (NodeDirection)
        {
        case EGridDirection::EGD_Up:
            EdgePosition = TargetVolumeLocation + FVector(TargetHalfHeight - SafeDistance, 0, SpawnLocation.Z);
            break;
        case EGridDirection::EGD_Down:
            EdgePosition = TargetVolumeLocation + FVector(-TargetHalfHeight + SafeDistance, 0, SpawnLocation.Z);
            break;
        case EGridDirection::EGD_Left:
            EdgePosition = TargetVolumeLocation + FVector(0, -TargetHalfWidth + SafeDistance, SpawnLocation.Z);
            break;
        case EGridDirection::EGD_Right:
            EdgePosition = TargetVolumeLocation + FVector(0, TargetHalfWidth - SafeDistance, SpawnLocation.Z);
            break;
        }
        
        FVector2D EdgePosition2D(EdgePosition.X, EdgePosition.Y);
        
        bool bIsInside = false;
        
        switch (NodeDirection)
        {
        case EGridDirection::EGD_Up:
            bIsInside = SpawnLocation.X < EdgePosition.X;
            break;
        case EGridDirection::EGD_Down:
            bIsInside = SpawnLocation.X > EdgePosition.X;
            break;
        case EGridDirection::EGD_Left:
            bIsInside = SpawnLocation.Y < EdgePosition.Y;
            break;
        case EGridDirection::EGD_Right:
            bIsInside = SpawnLocation.Y > EdgePosition.Y;
            break;
        }
        
        if (bIsInside)
        {
            TargetLocation = EdgePosition;
        }
        else
        {
            TargetLocation = SpawnLocation;
        }
        
        FRotator TargetRotation = TargetNode->SpawnPosition->GetComponentRotation();
        Character->SetActorRotation(TargetRotation);
    }
    else
    {
        AActor* OwningActor = GetOwner();
        if (!OwningActor) return;
        
        ASLGridVolume* CurrentVolume = Cast<ASLGridVolume>(OwningActor);
        if (!CurrentVolume) return;
        
        FVector CurrentVolumeLocation = CurrentVolume->GetActorLocation();
        FVector CharacterLocation = Character->GetActorLocation();
        
        FVector RelativeOffset;
        
        switch (NodeDirection)
        {
            case EGridDirection::EGD_Up:
            case EGridDirection::EGD_Down:
                RelativeOffset.X = 0;
                RelativeOffset.Y = CharacterLocation.Y - CurrentVolumeLocation.Y; 
                RelativeOffset.Z = CharacterLocation.Z - CurrentVolumeLocation.Z;
                break;
                
            case EGridDirection::EGD_Left:
            case EGridDirection::EGD_Right:
                RelativeOffset.X = CharacterLocation.X - CurrentVolumeLocation.X;
                RelativeOffset.Y = 0;
                RelativeOffset.Z = CharacterLocation.Z - CurrentVolumeLocation.Z;
                break;
        }

        const FVector TargetVolumeLocation = TargetVolume->GetActorLocation();

        const float TargetGridWidth = TargetVolume->GetGridWidth();
        const float TargetGridHeight = TargetVolume->GetGridHeight();
        const float TargetHalfWidth = TargetGridWidth * 0.5f;
        const float TargetHalfHeight = TargetGridHeight * 0.5f;
        
        FVector EdgePosition;
        switch (NodeDirection)
        {
        case EGridDirection::EGD_Up:
            EdgePosition = TargetVolumeLocation + FVector(TargetHalfHeight - SafeDistance, 0, 0);
            break;
        case EGridDirection::EGD_Down:
            EdgePosition = TargetVolumeLocation + FVector(-TargetHalfHeight + SafeDistance, 0, 0);
            break;
        case EGridDirection::EGD_Left:
            EdgePosition = TargetVolumeLocation + FVector(0, -TargetHalfWidth + SafeDistance, 0);
            break;
        case EGridDirection::EGD_Right:
            EdgePosition = TargetVolumeLocation + FVector(0, TargetHalfWidth - SafeDistance, 0);
            break;
        }

        TargetLocation = EdgePosition + RelativeOffset;
        
        float ClampedY = FMath::Clamp(TargetLocation.Y, 
                                     TargetVolumeLocation.Y - TargetHalfWidth + SafeDistance, 
                                     TargetVolumeLocation.Y + TargetHalfWidth - SafeDistance);
        float ClampedX = FMath::Clamp(TargetLocation.X, 
                                     TargetVolumeLocation.X - TargetHalfHeight + SafeDistance, 
                                     TargetVolumeLocation.X + TargetHalfHeight - SafeDistance);
        
        TargetLocation.Y = ClampedY;
        TargetLocation.X = ClampedX;
    }

    Character->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
    SwitchToVolumeCamera(PlayerController, TargetVolume);
}

void USLGridNode::SwitchToVolumeCamera(APlayerController* PlayerController, ASLGridVolume* TargetVolume)
{
    if (!PlayerController || !TargetVolume) return;
    
    UCameraComponent* TargetCamera = TargetVolume->GetCameraComponent();
    if (!TargetCamera) return;
    
    const FViewTargetTransitionParams TransitionParams;
    
    FMinimalViewInfo CameraInfo;
    TargetCamera->GetCameraView(0.f, CameraInfo);
    
    PlayerController->SetViewTarget(TargetVolume, TransitionParams);
}

