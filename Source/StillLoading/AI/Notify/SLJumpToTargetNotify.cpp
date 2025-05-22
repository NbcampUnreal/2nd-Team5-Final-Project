// SLJumpToTargetNotify.cpp
#include "SLJumpToTargetNotify.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// 스태틱 맵 초기화 (헤더에서 선언한 변수이므로 TObjectPtr 유지)
TMap<TObjectPtr<USkeletalMeshComponent>, FTimerHandle> USLJumpToTargetNotify::JumpTimerHandles;
TMap<TObjectPtr<USkeletalMeshComponent>, TEnumAsByte<ECollisionEnabled::Type>> USLJumpToTargetNotify::OriginalCollisionTypes;

USLJumpToTargetNotify::USLJumpToTargetNotify()
{
    bUpdateRotation = true;
    JumpTime = 0.0f;
    TargetBlackboardKey = "TargetActor"; // 기본 키 이름
    ArcHeightMultiplier = 1.0f;
}

void USLJumpToTargetNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);
    
    if (!MeshComp || !MeshComp->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: Invalid mesh component or owner"));
        return;
    }
        
    ACharacter* OwningCharacter = Cast<ACharacter>(MeshComp->GetOwner());
    if (!OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: Owner is not a Character"));
        return;
    }
    
    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: No AnimInstance found"));
        return;
    }
    
    AAIController* AIController = Cast<AAIController>(OwningCharacter->GetController());
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: No AIController found"));
        return;
    }
    
    AActor* TargetActor = nullptr;
    
    UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
    if (Blackboard)
    {
        TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetBlackboardKey));
    }
    
    // 타겟이 없으면 종료
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Jump failed: No valid target found for key '%s'"), *TargetBlackboardKey.ToString());
        return;
    }
    
    UCharacterMovementComponent* MovementComponent = OwningCharacter->GetCharacterMovement();
    
    // 출발 위치
    FVector StartLocation = OwningCharacter->GetActorLocation();
    
    // 타겟 위치
    FVector TargetLocation = TargetActor->GetActorLocation();
    
    ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
    if (TargetCharacter)
    {
        FVector TargetForward = TargetCharacter->GetActorForwardVector();
        TargetLocation -= TargetForward * 150.0f; // 타겟 앞 150cm에 착지
    }
    
    // 점프 방향 및 거리 계산
    FVector ToTarget = TargetLocation - StartLocation;
    FVector DirectionXY = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
    float DistanceXY = FVector::Dist2D(StartLocation, TargetLocation);
    
    // 캐릭터 회전
    if (bUpdateRotation)
    {
        FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionXY).Rotator();
        OwningCharacter->SetActorRotation(NewRotation);
    }
    
    // 물리 방정식을 사용하여 필요한 초기 속도 계산
    float Gravity = OwningCharacter->GetWorld()->GetGravityZ() * -1.0f;
    if (MovementComponent)
    {
        Gravity = MovementComponent->GetGravityZ() * -1.0f;
    }
    
    // 거리에 따라 점프 시간 동적 조정
    float BaseJumpTime = JumpTime > 0.0f ? JumpTime : 1.0f;
    
    // 거리에 따른 점프 시간 조정 - 멀수록 시간 증가
    float DistanceBasedAdjustment = FMath::Clamp(DistanceXY / 1000.0f, 0.0f, 3.0f);
    float EstimatedJumpTime = BaseJumpTime + DistanceBasedAdjustment;
    
    // 몽타주 남은 시간 확인 (JumpTime이 0일 경우)
    if (JumpTime <= 0.0f && AnimInstance->IsAnyMontagePlaying())
    {
        UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
        if (CurrentMontage)
        {
            float CurrentTime = AnimInstance->Montage_GetPosition(CurrentMontage);
            float TotalDuration = CurrentMontage->GetPlayLength();
            float RemainingTime = TotalDuration - CurrentTime;
            
            // 필요한 점프 시간이 몽타주 남은 시간보다 길면 점프 시간 조정
            if (EstimatedJumpTime > RemainingTime && RemainingTime > 0.2f)
            {
                // 몽타주 시간에 맞춰 점프 속도 증가
                EstimatedJumpTime = RemainingTime;
            }
        }
    }
    
    // 최소 시간 보장
    if (EstimatedJumpTime < 0.2f)
    {
        EstimatedJumpTime = 0.2f;
    }
    
    // 점프 시간의 85%를 이동에 사용
    float TimeToReachTarget = EstimatedJumpTime * 0.85f;
    
    // 필요한 수평 속도 계산 (속도 제한 없음)
    float RequiredHorizontalSpeed = DistanceXY / TimeToReachTarget;
    
    float HeightDifference = TargetLocation.Z - StartLocation.Z;
    
    // 포물선 높이 추가 - 거리가 멀면 더 높게 점프
    float ArcHeight = FMath::Clamp(DistanceXY / 10.0f, 50.0f, 400.0f) * ArcHeightMultiplier;
    
    // 중력을 고려한 수직 속도 계산 (추가 높이 포함)
    float Vz = (HeightDifference / TimeToReachTarget) + 
              (0.5f * Gravity * TimeToReachTarget) + 
              (2.0f * ArcHeight / TimeToReachTarget);
    
    // 최종 속도 벡터 계산
    FVector JumpVelocity = DirectionXY * RequiredHorizontalSpeed;
    JumpVelocity.Z = Vz;
    
    // 디버그 정보 출력
    UE_LOG(LogTemp, Display, TEXT("Jump Info - Distance: %.2f, Time: %.2f, Speed: %.2f, Height Diff: %.2f, Arc: %.2f"),
           DistanceXY, TimeToReachTarget, RequiredHorizontalSpeed, HeightDifference, ArcHeight);
    
    // 함수 내부에서 로컬 변수로 선언 - 일반 포인터 사용
    UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
    if (CharacterMesh)
    {
        // 충돌 비활성화
        TEnumAsByte<ECollisionEnabled::Type> OriginalCollision = CharacterMesh->GetCollisionEnabled();
        CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        
        // 원래 충돌 설정 저장 (헤더에 선언된 TObjectPtr 맵을 사용)
        if (OriginalCollisionTypes.Contains(MeshComp))
        {
            OriginalCollisionTypes[MeshComp] = OriginalCollision;
        }
        else
        {
            OriginalCollisionTypes.Add(MeshComp, OriginalCollision);
        }
    }
    
    // 함수 내부에서 로컬 변수로 선언 - 일반 포인터 사용
    UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
    
    // 점프 적용
    OwningCharacter->LaunchCharacter(JumpVelocity, true, true);
    
    // 이전 타이머가 있으면 제거
    if (JumpTimerHandles.Contains(MeshComp))
    {
        OwningCharacter->GetWorldTimerManager().ClearTimer(JumpTimerHandles[MeshComp]);
    }
    
    // 타이머를 설정하여 점프 종료 시 충돌 재활성화
    FTimerHandle NewTimerHandle;
    OwningCharacter->GetWorldTimerManager().SetTimer(
        NewTimerHandle,
        FTimerDelegate::CreateStatic(&USLJumpToTargetNotify::FinishJump, MeshComp),
        EstimatedJumpTime + 0.2f,
        false
    );
    
    if (JumpTimerHandles.Contains(MeshComp))
    {
        JumpTimerHandles[MeshComp] = NewTimerHandle;
    }
    else
    {
        JumpTimerHandles.Add(MeshComp, NewTimerHandle);
    }
}

// 점프 종료 스태틱 함수
void USLJumpToTargetNotify::FinishJump(USkeletalMeshComponent* MeshComp)
{
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("FinishJump: Invalid mesh component"));
        return;
    }
    
    ACharacter* OwningCharacter = Cast<ACharacter>(MeshComp->GetOwner());
    if (!OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("FinishJump: Invalid character owner"));
        return;
    }
    
    if (OriginalCollisionTypes.Contains(MeshComp))
    {
        UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
        if (CharacterMesh)
        {
            CharacterMesh->SetCollisionEnabled(OriginalCollisionTypes[MeshComp]);
            UE_LOG(LogTemp, Display, TEXT("Jump finished: Collision restored"));
        }
        
        // 맵에서 제거
        OriginalCollisionTypes.Remove(MeshComp);
    }
    
    UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    }
    
    if (JumpTimerHandles.Contains(MeshComp))
    {
        JumpTimerHandles.Remove(MeshComp);
    }
}