#include "SLBTService_2DOrientToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

USLBTService_2DOrientToTarget::USLBTService_2DOrientToTarget()
{
    NodeName = TEXT("2D Orient To Target (World Based)");
    INIT_SERVICE_NODE_NOTIFY_FLAGS();
    
    RotationSpeed = 360.0f;
    bInstantRotation = false;
    AngleChangeThreshold = 45.0f;
    RotationSmoothness = 0.85f;
    Interval = 0.1f;
    RandomDeviation = 0.0f;
    
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
}

void USLBTService_2DOrientToTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    
    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        TargetActorKey.ResolveSelectedKey(*BBAsset);
    }
}

FString USLBTService_2DOrientToTarget::GetStaticDescription() const
{
    const FString KeyDescription = TargetActorKey.SelectedKeyName.ToString();
    return FString::Printf(TEXT("2D World Orient to %s (90° snapping) %s"), *KeyDescription, *GetStaticServiceDescription());
}

void USLBTService_2DOrientToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
    UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(ActorObject);
    APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
    
    if (!OwningPawn || !TargetActor)
    {
        return;
    }
    
    ACharacter* Character = Cast<ACharacter>(OwningPawn);
    if (!Character || !Character->GetMesh())
    {
        return;
    }
    
    USkeletalMeshComponent* MeshComponent = Character->GetMesh();
    
    // 현재 메시의 상대 회전과 목표 회전 계산
    FRotator CurrentRotation = MeshComponent->GetRelativeRotation();
    float TargetYaw = CalculateWorldBasedTargetRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
    FRotator TargetRotation(0.0f, TargetYaw, 0.0f);
    
    // 회전이 필요한지 확인
    if (!ShouldUpdateRotation(CurrentRotation.Yaw, TargetYaw, AngleChangeThreshold))
    {
        return;
    }
    
    if (bInstantRotation)
    {
        MeshComponent->SetRelativeRotation(TargetRotation);
    }
    else
    {
        // 부드러운 회전을 위한 보간
        float InterpSpeed = RotationSpeed / 60.0f * (1.0f - RotationSmoothness);
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, InterpSpeed);
        
        // 90도에 가까워지면 스냅
        float NormalizedYaw = FRotator::NormalizeAxis(NewRotation.Yaw);
        float DeltaToSnap = FMath::Fmod(FMath::Abs(NormalizedYaw), 90.0f);
        
        if (DeltaToSnap < 5.0f || DeltaToSnap > 85.0f)
        {
            NewRotation.Yaw = FMath::RoundToFloat(NormalizedYaw / 90.0f) * 90.0f;
        }
        
        // Yaw만 사용 (2D 회전)
        NewRotation.Pitch = 0.0f;
        NewRotation.Roll = 0.0f;
        
        // SkeletalMeshComponent의 상대 회전 설정
        MeshComponent->SetRelativeRotation(NewRotation);
    }
}

float USLBTService_2DOrientToTarget::CalculateWorldBasedTargetRotation(const FVector& FromLocation, const FVector& ToLocation) const
{
    // 방향 벡터 계산
    FVector Direction = ToLocation - FromLocation;
    Direction.Z = 0.0f;
    
    if (Direction.IsNearlyZero())
    {
        return 0.0f;
    }
    
    Direction.Normalize();
    
    // FixingAxisComponent와 동일한 계산 방식 사용
    float YRotation = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X)) - 90.0f;
    float RoundedYRotation = FMath::RoundToFloat(YRotation / 90.0f) * 90.0f;
    
    return RoundedYRotation;
}

bool USLBTService_2DOrientToTarget::ShouldUpdateRotation(float CurrentYaw, float TargetYaw, float Threshold) const
{
    float AngleDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw));
    return AngleDifference >= Threshold;
}