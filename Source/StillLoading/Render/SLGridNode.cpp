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
	SpawnPosition = CreateDefaultSubobject<USLGridNodePosition>(FName(*CompName));
    SpawnPosition->OwnerNode = this;
}

void USLGridNode::UpdateTriggerVolume()
{
    if (!TriggerVolume) return;

    if (ConnectedVolume.IsValid())
    {
        TriggerVolume->SetCollisionProfileName(TEXT("Custom"));
        TriggerVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
    }
    else
    {
        TriggerVolume->SetCollisionProfileName(TEXT("Custom"));
        TriggerVolume->SetCollisionResponseToAllChannels(ECR_Block);
    }
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void USLGridNode::BeginPlay()
{
	Super::BeginPlay();
    
    UpdateTriggerVolume();

    // 시작하자마자 트리거에 걸려서 팅기는 현상 제어
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USLGridNode::AddTriggerDelegate, 0.1f, true);
}

#if WITH_EDITOR
void USLGridNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    UpdateTriggerVolume();
}
#endif

// 방향별 벡터를 반환하는 헬퍼 함수
FVector USLGridNode::GetDirectionVector(EGridDirection Direction) const
{
    switch (Direction)
    {
    case EGridDirection::EGD_Up:    return FVector(1.0f, 0.0f, 0.0f);
    case EGridDirection::EGD_Down:  return FVector(-1.0f, 0.0f, 0.0f);
    case EGridDirection::EGD_Left:  return FVector(0.0f, -1.0f, 0.0f);
    case EGridDirection::EGD_Right: return FVector(0.0f, 1.0f, 0.0f);
    default:
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: Unknown grid direction"));
        return FVector::ZeroVector;
    }
}

// 트리거 두께 계산
float USLGridNode::CalculateTriggerThickness(UBoxComponent* InTriggerVolume, EGridDirection Direction) const
{
    if (!TriggerVolume)
    {
        return 0.0f;
    }

    const FVector TriggerExtent = InTriggerVolume->GetScaledBoxExtent();
    
    switch (Direction)
    {
    case EGridDirection::EGD_Up:
    case EGridDirection::EGD_Down:
        return TriggerExtent.X * TriggerExtentMultiplier;
    case EGridDirection::EGD_Left:
    case EGridDirection::EGD_Right:
        return TriggerExtent.Y * TriggerExtentMultiplier;
    default:
        return 0.0f;
    }
}

// 안전 거리 계산 - 2D 이동용으로 수정
float USLGridNode::CalculateSafeDistance(const UCapsuleComponent* CharacterCapsule, float TriggerThickness, EGridDirection Direction) const
{
    if (!CharacterCapsule)
    {
        return SafeDistanceBuffer;
    }

    const float CapsuleRadius = CharacterCapsule->GetScaledCapsuleRadius();
    
    // 2D 이동이므로 캡슐 반지름만 고려하고 높이는 제외
    const float SafeDistance = TriggerThickness + CapsuleRadius + SafeDistanceBuffer;
    
    return SafeDistance;
}


// 엣지 위치 계산
FVector USLGridNode::CalculateEdgePosition(const ASLGridVolume* TargetVolume, EGridDirection Direction, float SafeDistance, float ZOffset) const
{
    if (!TargetVolume)
    {
        return FVector::ZeroVector;
    }

    const FVector TargetVolumeLocation = TargetVolume->GetActorLocation();
    const float TargetGridWidth = TargetVolume->GetGridWidth();
    const float TargetGridHeight = TargetVolume->GetGridHeight();
    const float TargetHalfWidth = TargetGridWidth * 0.5f;
    const float TargetHalfHeight = TargetGridHeight * 0.5f;
    
    switch (Direction)
    {
    case EGridDirection::EGD_Up:
        return TargetVolumeLocation + FVector(TargetHalfHeight - SafeDistance, 0.0f, ZOffset);
    case EGridDirection::EGD_Down:
        return TargetVolumeLocation + FVector(-TargetHalfHeight + SafeDistance, 0.0f, ZOffset);
    case EGridDirection::EGD_Left:
        return TargetVolumeLocation + FVector(0.0f, -TargetHalfWidth + SafeDistance, ZOffset);
    case EGridDirection::EGD_Right:
        return TargetVolumeLocation + FVector(0.0f, TargetHalfWidth - SafeDistance, ZOffset);
    default:
        return TargetVolumeLocation;
    }
}

// 스폰 위치 기반 타겟 위치 계산
FVector USLGridNode::CalculateSpawnBasedTargetLocation(const USLGridNode* TargetNode, const ASLGridVolume* TargetVolume, 
                                                       float SafeDistance, ACharacter* Character) const
{
    if (!TargetNode || !TargetNode->SpawnPosition || !TargetVolume || !Character)
    {
        return FVector::ZeroVector;
    }

    const FVector SpawnLocation = TargetNode->SpawnPosition->GetComponentLocation();
    const FVector EdgePosition = CalculateEdgePosition(TargetVolume, NodeDirection, SafeDistance, SpawnLocation.Z);
    
    // 스폰 위치가 볼륨 내부에 있는지 확인
    const bool bIsInside = IsSpawnLocationInside(SpawnLocation, EdgePosition, NodeDirection);
    
    const FVector TargetLocation = bIsInside ? EdgePosition : SpawnLocation;
    
    // 캐릭터 회전 설정
    const FRotator TargetRotation = TargetNode->SpawnPosition->GetComponentRotation();
    Character->SetActorRotation(TargetRotation);
    
    return TargetLocation;
}

// 스폰 위치가 볼륨 내부에 있는지 확인
bool USLGridNode::IsSpawnLocationInside(const FVector& SpawnLocation, const FVector& EdgePosition, EGridDirection Direction) const
{
    switch (Direction)
    {
    case EGridDirection::EGD_Up:
        return SpawnLocation.X > EdgePosition.X;
    case EGridDirection::EGD_Down:
        return SpawnLocation.X < EdgePosition.X;
    case EGridDirection::EGD_Left:
        return SpawnLocation.Y < EdgePosition.Y;
    case EGridDirection::EGD_Right:
        return SpawnLocation.Y > EdgePosition.Y;
    default:
        return false;
    }
}

// 상대적 오프셋 기반 타겟 위치 계산
FVector USLGridNode::CalculateRelativeOffsetTargetLocation(const ACharacter* Character, const ASLGridVolume* CurrentVolume, 
                                                           const ASLGridVolume* TargetVolume, float SafeDistance) const
{
    if (!Character || !CurrentVolume || !TargetVolume)
    {
        return FVector::ZeroVector;
    }

    const FVector CurrentVolumeLocation = CurrentVolume->GetActorLocation();
    const FVector CharacterLocation = Character->GetActorLocation();
    
    // 상대적 오프셋 계산
    FVector RelativeOffset = CalculateRelativeOffset(CharacterLocation, CurrentVolumeLocation, NodeDirection);
    
    // 엣지 위치 계산
    const FVector EdgePosition = CalculateEdgePosition(TargetVolume, NodeDirection, SafeDistance);
    
    // 최종 위치 계산 및 클램핑
    FVector TargetLocation = EdgePosition + RelativeOffset;
    ClampLocationToBounds(TargetLocation, TargetVolume, SafeDistance);
    
    return TargetLocation;
}

// 상대적 오프셋 계산
FVector USLGridNode::CalculateRelativeOffset(const FVector& CharacterLocation, const FVector& VolumeLocation, EGridDirection Direction) const
{
    FVector RelativeOffset = FVector::ZeroVector;
    
    switch (Direction)
    {
    case EGridDirection::EGD_Up:
    case EGridDirection::EGD_Down:
        RelativeOffset.Y = CharacterLocation.Y - VolumeLocation.Y;
        RelativeOffset.Z = CharacterLocation.Z - VolumeLocation.Z;
        break;
    case EGridDirection::EGD_Left:
    case EGridDirection::EGD_Right:
        RelativeOffset.X = CharacterLocation.X - VolumeLocation.X;
        RelativeOffset.Z = CharacterLocation.Z - VolumeLocation.Z;
        break;
    }
    
    return RelativeOffset;
}

// 위치를 볼륨 경계 내로 클램핑
void USLGridNode::ClampLocationToBounds(FVector& Location, const ASLGridVolume* TargetVolume, float SafeDistance) const
{
    if (!TargetVolume)
    {
        return;
    }

    const FVector TargetVolumeLocation = TargetVolume->GetActorLocation();
    const float TargetGridWidth = TargetVolume->GetGridWidth();
    const float TargetGridHeight = TargetVolume->GetGridHeight();
    const float TargetHalfWidth = TargetGridWidth * 0.5f;
    const float TargetHalfHeight = TargetGridHeight * 0.5f;
    
    Location.Y = FMath::Clamp(Location.Y, 
                             TargetVolumeLocation.Y - TargetHalfWidth + SafeDistance, 
                             TargetVolumeLocation.Y + TargetHalfWidth - SafeDistance);
    Location.X = FMath::Clamp(Location.X, 
                             TargetVolumeLocation.X - TargetHalfHeight + SafeDistance, 
                             TargetVolumeLocation.X + TargetHalfHeight - SafeDistance);
}

// 필수 컴포넌트들 검증
bool USLGridNode::ValidateRequiredComponents(ACharacter* Character, APlayerController*& OutPlayerController, 
                                            UCapsuleComponent*& OutCapsuleComponent, ASLGridVolume*& OutTargetVolume, 
                                            USLGridNode*& OutTargetNode) const
{
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: Invalid character"));
        return false;
    }

    OutCapsuleComponent = Character->GetCapsuleComponent();
    if (!OutCapsuleComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: Character has no capsule component"));
        return false;
    }
    
    OutPlayerController = Cast<APlayerController>(Character->GetController());
    if (!OutPlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: Character has no player controller"));
        return false;
    }

    OutTargetVolume = ConnectedVolume.Get();
    if (!OutTargetVolume)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: No connected volume"));
        return false;
    }

    OutTargetNode = OutTargetVolume->GetNodeByDirection(NodeDirection);
    if (!OutTargetNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("USLGridNode: No target node found for direction"));
        return false;
    }

    return true;
}

// 메인 트리거 함수 - 리팩토링됨
void USLGridNode::OnTriggeredNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐스팅 및 검증
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    APlayerController* PlayerController = nullptr;
    UCapsuleComponent* CharacterCapsule = nullptr;
    ASLGridVolume* TargetVolume = nullptr;
    USLGridNode* TargetNode = nullptr;
    
    if (!ValidateRequiredComponents(Character, PlayerController, CharacterCapsule, TargetVolume, TargetNode))
    {
        return;
    }
    
    // 트리거 두께 및 안전 거리 계산
    const float TriggerThickness = CalculateTriggerThickness(TargetNode->TriggerVolume, NodeDirection);
    const float SafeDistance = CalculateSafeDistance(CharacterCapsule, TriggerThickness, NodeDirection);
    
    // 타겟 위치 계산
    FVector TargetLocation;
    if (bUseSpawnPosition && TargetNode->SpawnPosition)
    {
        TargetLocation = CalculateSpawnBasedTargetLocation(TargetNode, TargetVolume, SafeDistance, Character);
    }
    else
    {
        // 현재 볼륨 정보 가져오기
        AActor* OwningActor = GetOwner();
        if (!OwningActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("USLGridNode: No owning actor"));
            return;
        }
        
        ASLGridVolume* CurrentVolume = Cast<ASLGridVolume>(OwningActor);
        if (!CurrentVolume)
        {
            UE_LOG(LogTemp, Warning, TEXT("USLGridNode: Owning actor is not a grid volume"));
            return;
        }
        
        TargetLocation = CalculateRelativeOffsetTargetLocation(Character, CurrentVolume, TargetVolume, SafeDistance);
    }

    // 캐릭터 이동 및 카메라 전환
    Character->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
    SwitchToVolumeCamera(PlayerController, TargetVolume);
}

void USLGridNode::AddTriggerDelegate()
{
    TriggerVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnTriggeredNode);
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

