#include "SLBTTask_2DRotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

USLBTTask_2DRotateToTarget::USLBTTask_2DRotateToTarget()
{
    NodeName = TEXT("2D Rotate to Target (World Based)");
    RotationSpeed = 360.0f;
    AngleTolerance = 5.0f;
    MaxRotationTime = 2.0f;
    bInstantRotation = false;
    
    bNotifyTick = true;
    bNotifyTaskFinished = true;
    bCreateNodeInstance = false;
    
    INIT_TASK_NODE_NOTIFY_FLAGS();
    
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActorKey), AActor::StaticClass());
}

void USLBTTask_2DRotateToTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    
    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        TargetActorKey.ResolveSelectedKey(*BBAsset);
    }
}

uint16 USLBTTask_2DRotateToTarget::GetInstanceMemorySize() const
{
    return sizeof(F2DRotateTaskMemory);
}

FString USLBTTask_2DRotateToTarget::GetStaticDescription() const
{
    const FString KeyDescription = TargetActorKey.SelectedKeyName.ToString();
    return FString::Printf(TEXT("2D World Rotate to %s (90° snapping)"), *KeyDescription);
}

EBTNodeResult::Type USLBTTask_2DRotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    
    UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(ActorObject);
    APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
    
    F2DRotateTaskMemory* Memory = CastInstanceNodeMemory<F2DRotateTaskMemory>(NodeMemory);
    check(Memory);
    
    Memory->OwningPawn = OwningPawn;
    Memory->TargetActor = TargetActor;
    Memory->ElapsedTime = 0.0f;
    
    if (!Memory->IsValid())
    {
        return EBTNodeResult::Failed;
    }
    
    ACharacter* Character = Cast<ACharacter>(OwningPawn);
    if (!Character || !Character->GetMesh())
    {
        return EBTNodeResult::Failed;
    }
    
    // 현재 메시의 상대 회전 저장
    Memory->StartRotation = Character->GetMesh()->GetRelativeRotation();
    
    // 타겟 방향 계산
    float TargetYaw = CalculateWorldBasedTargetRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
    Memory->TargetRotation = FRotator(0.0f, TargetYaw, 0.0f);
    
    // 즉시 회전 옵션
    if (bInstantRotation)
    {
        Character->GetMesh()->SetRelativeRotation(Memory->TargetRotation);
        Memory->Reset();
        return EBTNodeResult::Succeeded;
    }
    
    // 이미 목표 각도에 도달한 경우
    if (IsAtTargetRotation(Memory->StartRotation.Yaw, Memory->TargetRotation.Yaw, AngleTolerance))
    {
        Memory->Reset();
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::InProgress;
}

void USLBTTask_2DRotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    
    F2DRotateTaskMemory* Memory = CastInstanceNodeMemory<F2DRotateTaskMemory>(NodeMemory);
    
    Memory->ElapsedTime += DeltaSeconds;
    
    if (!Memory->IsValid())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    // Character로 캐스팅하여 SkeletalMeshComponent 가져오기
    ACharacter* Character = Cast<ACharacter>(Memory->OwningPawn.Get());
    if (!Character)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    USkeletalMeshComponent* MeshComponent = Character->GetMesh();
    if (!MeshComponent)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    if (Memory->ElapsedTime >= MaxRotationTime)
    {
        // SkeletalMeshComponent의 상대 회전 설정
        MeshComponent->SetRelativeRotation(Memory->TargetRotation);
        Memory->Reset();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    // 현재 메시의 상대 회전 가져오기
    FRotator CurrentRotation = MeshComponent->GetRelativeRotation();
    
    // 목표에 도달했는지 확인
    if (IsAtTargetRotation(CurrentRotation.Yaw, Memory->TargetRotation.Yaw, AngleTolerance))
    {
        Memory->Reset();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    // 회전 보간
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, Memory->TargetRotation, DeltaSeconds, RotationSpeed / 60.0f);
    
    // Yaw만 사용 (2D 회전)
    NewRotation.Pitch = 0.0f;
    NewRotation.Roll = 0.0f;
    
    // SkeletalMeshComponent의 상대 회전 설정
    MeshComponent->SetRelativeRotation(NewRotation);
}

float USLBTTask_2DRotateToTarget::CalculateWorldBasedTargetRotation(const FVector& FromLocation, const FVector& ToLocation) const
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

bool USLBTTask_2DRotateToTarget::IsAtTargetRotation(float CurrentYaw, float TargetYaw, float Tolerance) const
{
    float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
    return FMath::Abs(DeltaYaw) <= Tolerance;
}