// Fill out your copyright notice in the Description page of Project Settings.


#include "SLBossAnimInstance.h"

#include "Animation/AnimInstanceProxy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

void USLBossAnimInstance::SetBossAttackPattern(EBossAttackPattern NewBossAttackPattern)
{
	BossAttackPattern = NewBossAttackPattern;
}

EBossAttackPattern USLBossAnimInstance::GetBossAttackPattern()
{
	return BossAttackPattern;
}

bool USLBossAnimInstance::CleanupThrowActor()
{
    if (!ActorToThrow)
    {
        // 제거할 액터가 없음
        return false;
    }
    
    // 액터가 어태치되어 있는지 확인
    USceneComponent* RootComp = ActorToThrow->GetRootComponent();
    if (RootComp && RootComp->GetAttachParent() != nullptr)
    {
        // 디태치
        RootComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    }
    
    // 액터 제거
    ActorToThrow->Destroy();
    
    // 참조 초기화
    ActorToThrow = nullptr;
    
    UE_LOG(LogTemp, Display, TEXT("Throw actor cleaned up successfully"));
    return true;
}

AActor* USLBossAnimInstance::SpawnActorToThrow(TSubclassOf<AActor> ActorClass, FName SocketName)
{
    if (!ActorClass || !OwningCharacter || !GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn actor: Invalid actor class or owner"));
        return nullptr;
    }
    
    if (!OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist on character mesh"), *SocketName.ToString());
        return nullptr;
    }
    
    FVector SpawnLocation = OwningCharacter->GetActorLocation(); 
    FRotator SpawnRotation = OwningCharacter->GetActorRotation();
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = OwningCharacter;
    
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (SpawnedActor)
    {
        // 루트 컴포넌트 가져오기
        USceneComponent* RootComp = SpawnedActor->GetRootComponent();
        if (RootComp)
        {
            // 물리 설정
            UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComp);
            if (PrimComp)
            {
                PrimComp->SetSimulatePhysics(false);
                PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            
            // 소켓에 어태치
            RootComp->AttachToComponent(
                OwningCharacter->GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                SocketName
            );
            
            // 위치 및 회전 재설정
            RootComp->SetRelativeLocation(FVector::ZeroVector);
            RootComp->SetRelativeRotation(FRotator::ZeroRotator);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned actor does not have a valid root component"));
        }
        
        // 생성된 액터 저장
        ActorToThrow = SpawnedActor;
    }
    
    return SpawnedActor;
}

AActor* USLBossAnimInstance::ThrowActorAtTarget(float LaunchSpeed, float TimeToTarget, FName SocketName)
{
    if (!ActorToThrow || !TargetCharacter || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot throw actor: Missing actor to throw or target"));
        return nullptr;
    }
    
    // 소켓 존재 여부 확인
    if (!OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist on character mesh"), *SocketName.ToString());
        return nullptr;
    }
    
    FVector StartLocation = OwningCharacter->GetMesh()->GetSocketLocation(SocketName);

    // 액터 디태치 및 위치 설정
    USceneComponent* RootComp = ActorToThrow->GetRootComponent();
    if (RootComp && RootComp->GetAttachParent() != nullptr)
    {
        // 물리 시뮬레이션 일시 중지
        UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComp);
        if (PrimComp)
        {
            PrimComp->SetSimulatePhysics(false);
            PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    
        // 디태치
        RootComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    
        ActorToThrow->SetActorLocation(StartLocation);
    
        // 물리 시뮬레이션 활성화
        if (PrimComp)
        {
            PrimComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
            PrimComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        
            PrimComp->SetSimulatePhysics(true);
            PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            PrimComp->IgnoreActorWhenMoving(OwningCharacter, true);
        }
    }
    
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    
    if (UCapsuleComponent* CapsuleComp = TargetCharacter->FindComponentByClass<UCapsuleComponent>())
    {
        float HalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
        TargetLocation.Z += HalfHeight * 0.7f; // 머리 높이 타겟팅
    }
    else
    {
        TargetLocation.Z += 100.0f; // 캡슐이 없는 경우 임의 높이 추가
    }
    
    // 시간 확인 (최소값 보장)
    TimeToTarget = FMath::Max(TimeToTarget, 0.5f);
    
    // 프로젝타일 컴포넌트 설정
    UProjectileMovementComponent* ProjectileMovement = ActorToThrow->FindComponentByClass<UProjectileMovementComponent>();
    if (ProjectileMovement)
    {
        ProjectileMovement->DestroyComponent();
    }
    
    ProjectileMovement = NewObject<UProjectileMovementComponent>(ActorToThrow, UProjectileMovementComponent::StaticClass());
    if (!ProjectileMovement)
    {
        return nullptr;
    }
    ProjectileMovement->RegisterComponent();
    
    // 물리 설정
    UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(ActorToThrow->GetRootComponent());
    if (PrimComp)
    {
        PrimComp->SetSimulatePhysics(true);
        PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        PrimComp->IgnoreActorWhenMoving(OwningCharacter, true);
    }
    
    // 중력 값
    float GravityZ = FMath::Abs(GetWorld()->GetGravityZ());
    if (GravityZ < KINDA_SMALL_NUMBER)
    {
        GravityZ = 980.0f;
    }
    
    // ======== 도달 시간 기반 속도 계산 ========
    FVector PositionDelta = TargetLocation - StartLocation;
    
    // 수평 속도: 거리/시간
    FVector HorizontalVelocity = FVector(PositionDelta.X, PositionDelta.Y, 0.0f) / TimeToTarget;
    
    // 수직 속도: 중력 보정 포함
    float VerticalVelocity = PositionDelta.Z / TimeToTarget + 0.5f * GravityZ * TimeToTarget;
    
    // 최종 발사 속도
    FVector LaunchVelocity = HorizontalVelocity;
    LaunchVelocity.Z = VerticalVelocity;
    
    // 최대 속도 제한
    float CalculatedSpeed = LaunchVelocity.Size();
    if (CalculatedSpeed > LaunchSpeed)
    {
        LaunchVelocity = LaunchVelocity * (LaunchSpeed / CalculatedSpeed);
        UE_LOG(LogTemp, Display, TEXT("Velocity scaled down to stay within limit. Consider increasing TimeToTarget."));
    }
    
    // 프로젝타일 설정 적용
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 1.0f;
    ProjectileMovement->Velocity = LaunchVelocity;
    ProjectileMovement->InitialSpeed = LaunchVelocity.Size();
    ProjectileMovement->MaxSpeed = LaunchVelocity.Size() * 1.1f;
    
    // 디버그 정보
    UE_LOG(LogTemp, Display, TEXT("Throw: Distance=%.1f, Time=%.2fs, Velocity=(%.1f,%.1f,%.1f)"),
        PositionDelta.Size(), TimeToTarget, LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z);
    
    // 액터 던진 후 참조 해제
    AActor* ThrownActor = ActorToThrow;
    ActorToThrow = nullptr;
    
    return ThrownActor;
}

bool USLBossAnimInstance::JumpToTarget(bool bUpdateRotation, float RemainingAnimTime)
{
    // 타겟과 소유 캐릭터가 유효한지 확인
    if (!TargetCharacter || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot jump to target: Missing target or owner"));
        return false;
    }
    
    // ★ 1단계: 물리적으로 필요한 점프 시간 계산 ★
    
    // 출발 위치와 타겟 위치
    FVector StartLocation = OwningCharacter->GetActorLocation();
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    FVector TargetForward = TargetCharacter->GetActorForwardVector();
    TargetLocation -= TargetForward * 150.0f; // 타겟 앞 150cm에 착지
    
    // 점프 방향 및 거리 계산
    FVector ToTarget = TargetLocation - StartLocation;
    FVector DirectionXY = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
    float DistanceXY = FVector::Dist2D(StartLocation, TargetLocation);
    float HeightDifference = TargetLocation.Z - StartLocation.Z;
    
    // 중력값 계산
    float Gravity = GetWorld()->GetGravityZ() * -1.0f;
    if (OwningMovementComponent)
    {
        Gravity = OwningMovementComponent->GetGravityZ() * -1.0f;
    }
    
    // ★ 물리적으로 최적의 점프 시간 계산 ★
    float OptimalJumpTime;
    if (HeightDifference >= 0) // 위로 점프하거나 같은 높이
    {
        // 포물선 운동으로 최적 시간 계산
        float DiscriminantHeight = HeightDifference + (Gravity * 0.25f); // 약간의 높이 여유
        OptimalJumpTime = FMath::Sqrt(2.0f * DiscriminantHeight / Gravity);
        OptimalJumpTime = FMath::Max(OptimalJumpTime, 0.8f); // 최소 시간
    }
    else // 아래로 점프
    {
        // 하강 시간 기반 계산
        OptimalJumpTime = FMath::Sqrt(2.0f * FMath::Abs(HeightDifference) / Gravity);
        OptimalJumpTime = FMath::Max(OptimalJumpTime, 0.6f);
    }
    
    // 거리에 따른 시간 조정
    float DistanceBasedTime = DistanceXY / 800.0f; // 기본 속도 800 기준
    OptimalJumpTime = FMath::Max(OptimalJumpTime, DistanceBasedTime);
    OptimalJumpTime = FMath::Clamp(OptimalJumpTime, 0.8f, 3.0f); // 합리적 범위로 제한
    
    UE_LOG(LogTemp, Display, TEXT("Calculated optimal jump time: %f seconds"), OptimalJumpTime);
    
    // ★ 2단계: 애니메이션 시간과 비교 및 조정 ★
    
    float AnimationTime = OptimalJumpTime; // 기본적으로 물리 시간 사용
    
    if (RemainingAnimTime > 0.0f)
    {
        AnimationTime = RemainingAnimTime;
    }
    else if (IsAnyMontagePlaying())
    {
        // 현재 재생 중인 몽타주의 남은 시간 계산
        FAnimMontageInstance* MontageInstance = GetActiveMontageInstance();
        if (MontageInstance && MontageInstance->Montage)
        {
            float CurrentTime = MontageInstance->GetPosition();
            float TotalDuration = MontageInstance->Montage->GetPlayLength();
            AnimationTime = TotalDuration - CurrentTime;
        }
    }
    
    // ★ 3단계: 애니메이션과 점프 시간 동기화 결정 ★
    
    float FinalJumpTime;
    bool bAdjustAnimationSpeed = false;
    
    float TimeDifference = FMath::Abs(OptimalJumpTime - AnimationTime);
    
    if (TimeDifference < 0.3f) // 시간 차이가 작으면
    {
        // 물리 시간 우선 (더 자연스러운 점프)
        FinalJumpTime = OptimalJumpTime;
        bAdjustAnimationSpeed = true;
        UE_LOG(LogTemp, Display, TEXT("Using physics time, will adjust animation speed"));
    }
    else if (AnimationTime > OptimalJumpTime * 1.5f) // 애니메이션이 너무 길면
    {
        // 물리 시간 사용하고 애니메이션 속도 증가
        FinalJumpTime = OptimalJumpTime;
        bAdjustAnimationSpeed = true;
        UE_LOG(LogTemp, Display, TEXT("Animation too long, using physics time"));
    }
    else if (AnimationTime < OptimalJumpTime * 0.6f) // 애니메이션이 너무 짧으면
    {
        // 물리 시간 사용하고 애니메이션 속도 감소
        FinalJumpTime = OptimalJumpTime;
        bAdjustAnimationSpeed = true;
        UE_LOG(LogTemp, Display, TEXT("Animation too short, using physics time"));
    }
    else
    {
        // 애니메이션 시간에 맞춰 점프 조정
        FinalJumpTime = AnimationTime;
        UE_LOG(LogTemp, Display, TEXT("Using animation time for jump"));
    }
    
    FinalJumpTime = FMath::Clamp(FinalJumpTime, 0.5f, 4.0f);
    
    // ★ 4단계: 애니메이션 속도 조정 (필요한 경우) ★
    
    if (bAdjustAnimationSpeed && IsAnyMontagePlaying())
    {
        float NewPlayRate = AnimationTime / FinalJumpTime;
        NewPlayRate = FMath::Clamp(NewPlayRate, 0.5f, 2.0f); // 속도 제한
        
        FAnimMontageInstance* MontageInstance = GetActiveMontageInstance();
        if (MontageInstance)
        {
            MontageInstance->SetPlayRate(NewPlayRate);
            UE_LOG(LogTemp, Display, TEXT("Adjusted animation play rate to: %f"), NewPlayRate);
        }
    }
    
    // ★ 5단계: 점프 속도 계산 (최종 시간 기준) ★
    
    // 캐릭터 회전
    if (bUpdateRotation)
    {
        FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionXY).Rotator();
        OwningCharacter->SetActorRotation(NewRotation);
    }
    
    // 수평 속도 계산
    float RequiredHorizontalSpeed = DistanceXY / FinalJumpTime;
    
    // 수직 속도 계산 (중력 고려)
    float Vz = (HeightDifference / FinalJumpTime) + (0.5f * Gravity * FinalJumpTime);
    
    // 최종 속도 벡터
    FVector JumpVelocity = DirectionXY * RequiredHorizontalSpeed;
    JumpVelocity.Z = Vz;
    
    // 속도 제한 (안전장치)
    float MaxSpeed = 2000.0f;
    float MinSpeed = 200.0f;
    
    float CurrentSpeed = JumpVelocity.Size();
    if (CurrentSpeed > MaxSpeed)
    {
        JumpVelocity = JumpVelocity.GetSafeNormal() * MaxSpeed;
        UE_LOG(LogTemp, Warning, TEXT("Jump velocity capped at max speed"));
    }
    else if (CurrentSpeed < MinSpeed)
    {
        JumpVelocity = JumpVelocity.GetSafeNormal() * MinSpeed;
        UE_LOG(LogTemp, Warning, TEXT("Jump velocity boosted to min speed"));
    }
    
    UE_LOG(LogTemp, Display, TEXT("Final jump velocity: %s (speed: %f)"), *JumpVelocity.ToString(), JumpVelocity.Size());
    UE_LOG(LogTemp, Display, TEXT("Final jump time: %f seconds"), FinalJumpTime);
    
    // ★ 6단계: 충돌 설정 및 점프 실행 ★
    
    // 캐릭터 충돌 비활성화
    UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
    if (CharacterMesh)
    {
        OriginalCollisionType = CharacterMesh->GetCollisionEnabled();
        CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    // 캡슐 컴포넌트 충돌 설정
    UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
    
    // 점프 실행
    OwningCharacter->LaunchCharacter(JumpVelocity, true, true);
    
    // ★ 7단계: 정확한 타이밍으로 점프 종료 설정 ★
    
    // 약간의 여유 시간 추가 (착지 후 안정화)
    float FinishDelay = FinalJumpTime + 0.1f;
    
    OwningCharacter->GetWorldTimerManager().SetTimer(
        JumpTimerHandle,
        this,
        &USLBossAnimInstance::FinishJump,
        FinishDelay,
        false
    );
    
    UE_LOG(LogTemp, Display, TEXT("Jump initiated! Will finish in %f seconds"), FinishDelay);
    
    return true;
}

void USLBossAnimInstance::FinishJump()
{
    UE_LOG(LogTemp, Display, TEXT("Jump finished - restoring collision"));
    
    // 충돌 설정 복원
    if (OwningCharacter)
    {
        UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
        if (CharacterMesh)
        {
            CharacterMesh->SetCollisionEnabled(OriginalCollisionType);
        }
        
        UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
        if (CapsuleComp)
        {
            CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        }
        
        // 타이머 핸들 클리어
        if (JumpTimerHandle.IsValid())
        {
            OwningCharacter->GetWorldTimerManager().ClearTimer(JumpTimerHandle);
        }
        
        // ★ 애니메이션 재생 속도 원래대로 복원 ★
        if (IsAnyMontagePlaying())
        {
            FAnimMontageInstance* MontageInstance = GetActiveMontageInstance();
            if (MontageInstance)
            {
                MontageInstance->SetPlayRate(1.0f); // 원래 속도로 복원
                UE_LOG(LogTemp, Display, TEXT("Animation play rate restored to 1.0"));
            }
        }
    }
}

bool USLBossAnimInstance::ChargeToTarget(float ChargeSpeed, bool bUpdateRotation)
{
    // 타겟과 소유 캐릭터가 유효한지 확인
    if (!TargetCharacter || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot charge to target: Missing target or owner"));
        return false;
    }
    
    // 출발 위치
    FVector StartLocation = OwningCharacter->GetActorLocation();
    
    // 타겟 위치 (조정 없이 직접 타겟 위치 사용)
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    
    // 돌진 방향 계산 - Z축은 무시하고 XY 평면에서만 방향 계산
    FVector ToTarget = TargetLocation - StartLocation;
    FVector DirectionXY = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
    
    // 캐릭터 회전 (타겟을 향하도록)
    if (bUpdateRotation)
    {
        FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionXY).Rotator();
        OwningCharacter->SetActorRotation(NewRotation);
    }
    
    // 캐릭터 충돌 비활성화 - 오버랩 방지
    UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
    if (CharacterMesh)
    {
        // 현재 충돌 설정 저장
        OriginalCollisionType = CharacterMesh->GetCollisionEnabled();
        
        // 충돌 비활성화 (쿼리는 유지하되 물리적 충돌은 비활성화)
        CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    // 캡슐 컴포넌트 충돌 설정
    UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
    if (CapsuleComp)
    {
        // 캐릭터 간 충돌만 무시하도록 설정 (다른 월드 충돌은 유지)
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
    
    // 돌진 적용 - 방식 1: LaunchCharacter (강한 돌진, 초기 추진력만 적용)
    FVector ChargeVelocity = DirectionXY * ChargeSpeed;
    // Z 속도는 유지 (중력의 영향으로 떨어지도록)
    ChargeVelocity.Z = 0.0f;
    OwningCharacter->LaunchCharacter(ChargeVelocity, true, true);
    
    // 디버그 정보 출력
    UE_LOG(LogTemp, Display, TEXT("Charge started - Direction: (%f, %f, %f), Speed: %.2f"),
           DirectionXY.X, DirectionXY.Y, DirectionXY.Z, ChargeSpeed);
    
    return true;
}

void USLBossAnimInstance::FinishCharge()
{
    // 충돌 설정 복원
    if (OwningCharacter)
    {
        UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
        if (CharacterMesh)
        {
            // 원래 충돌 설정으로 복원
            CharacterMesh->SetCollisionEnabled(OriginalCollisionType);
        }
        
        // 캡슐 컴포넌트 충돌 복원
        UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
        if (CapsuleComp)
        {
            // 캐릭터 간 충돌 다시 활성화
            CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        }
    }
    
    // 디버그 정보 출력
    UE_LOG(LogTemp, Display, TEXT("Charge finished"));
}





