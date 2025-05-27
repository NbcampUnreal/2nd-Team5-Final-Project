// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGridNode.h"

#include "SLGridVolume.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"


USLGridNode::USLGridNode()
{
	PrimaryComponentTick.bCanEverTick = false;
    
	FString CompName = GetName() + "_TriggerVolume";
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName(*CompName));
    
	CompName = GetName() + "_SpawnPosition";
	SpawnPosition = CreateDefaultSubobject<UBillboardComponent>(FName(*CompName));
}

void USLGridNode::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &USLGridNode::OnTriggeredNode);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &USLGridNode::OnExitTriggerNode);
}

void USLGridNode::OnTriggeredNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsUsable) return;
    
    APawn* Character = Cast<APawn>(OtherActor);
    if (!Character) return;

    APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
    if (!PlayerController) return;

    ASLGridVolume* TargetVolume = ConnectedVolume.Get();
    if (!TargetVolume) return;

    USLGridNode* TargetNode = TargetVolume->GetNodeByDirection(NodeDirection);
    if (!TargetNode) return;
    
    FVector TargetLocation;
    
    if (bUseSpawnPosition && TargetNode->SpawnPosition)
    {
        TargetLocation = TargetNode->SpawnPosition->GetComponentLocation();
        
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
                EdgePosition = TargetVolumeLocation + FVector(TargetHalfHeight, 0, 0);
                break;
            case EGridDirection::EGD_Down:
                EdgePosition = TargetVolumeLocation + FVector(-TargetHalfHeight, 0, 0);
                break;
            case EGridDirection::EGD_Left:
                EdgePosition = TargetVolumeLocation + FVector(0, -TargetHalfWidth, 0);
                break;
            case EGridDirection::EGD_Right:
                EdgePosition = TargetVolumeLocation + FVector(0, TargetHalfWidth, 0);
                break;
        }
        TargetLocation = EdgePosition + RelativeOffset;
    }

    TargetNode->bIsUsable = false;
    Character->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
    SwitchToVolumeCamera(PlayerController, TargetVolume);
    
}

void USLGridNode::OnExitTriggerNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    const APawn* Character = Cast<APawn>(OtherActor);
    if (!Character) return;
    
    bIsUsable = true;
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

