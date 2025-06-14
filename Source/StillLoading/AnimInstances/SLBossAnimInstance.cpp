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
    
    return true;
}

AActor* USLBossAnimInstance::SpawnActorToThrow(TSubclassOf<AActor> ActorClass, FName SocketName)
{
    if (!ActorClass || !OwningCharacter || !GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn actor: Invalid parameters"));
        return nullptr;
    }
    
    // 스폰 위치는 소켓 위치로
    FVector SpawnLocation = OwningCharacter->GetMesh()->GetSocketLocation(SocketName);
    FRotator SpawnRotation = OwningCharacter->GetActorRotation();
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Owner = OwningCharacter;
    
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (SpawnedActor)
    {
        // 소켓에 어태치
        SpawnedActor->AttachToComponent(
            OwningCharacter->GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            SocketName
        );
        
        // 충돌 비활성화 (던질 때까지)
        if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(SpawnedActor->GetRootComponent()))
        {
            PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        
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
    
    // 현재 액터의 위치 (이미 소켓에 어태치되어 있음)
    FVector StartLocation = ActorToThrow->GetActorLocation();
    
    // 소켓이 지정되어 있고 유효한 경우, 소켓 위치 사용
    if (SocketName != NAME_None && OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
    {
        StartLocation = OwningCharacter->GetMesh()->GetSocketLocation(SocketName);
    }
    
    // 액터 디태치
    USceneComponent* RootComp = ActorToThrow->GetRootComponent();
    if (RootComp && RootComp->GetAttachParent() != nullptr)
    {
        RootComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }
    
    // 디태치 후 정확한 위치로 설정
    ActorToThrow->SetActorLocation(StartLocation);
    
    // 타겟 위치 계산
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    if (UCapsuleComponent* CapsuleComp = TargetCharacter->FindComponentByClass<UCapsuleComponent>())
    {
        TargetLocation.Z += CapsuleComp->GetScaledCapsuleHalfHeight() * 0.5f; // 가슴 높이
    }
    
    // ProjectileMovementComponent 가져오기
    UProjectileMovementComponent* ProjectileMovement = ActorToThrow->FindComponentByClass<UProjectileMovementComponent>();
    if (!ProjectileMovement)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor does not have ProjectileMovementComponent"));
        ActorToThrow = nullptr;
        return nullptr;
    }
    
    // 초기 발사 방향 계산 (약간 위쪽으로)
    FVector Direction = TargetLocation - StartLocation;
    Direction.Normalize();
    
    // 위쪽으로 각도 추가 (15~30도)
    float UpwardAngle = FMath::DegreesToRadians(20.0f);
    float CurrentPitch = FMath::Atan2(Direction.Z, FVector(Direction.X, Direction.Y, 0).Size());
    float NewPitch = CurrentPitch + UpwardAngle;
    
    FVector LaunchDirection;
    LaunchDirection.X = Direction.X * FMath::Cos(UpwardAngle);
    LaunchDirection.Y = Direction.Y * FMath::Cos(UpwardAngle);
    LaunchDirection.Z = FMath::Sin(NewPitch);
    LaunchDirection.Normalize();
    
    // Owner 무시 설정
    if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComp))
    {
        PrimComp->IgnoreActorWhenMoving(OwningCharacter, true);
        PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    // ProjectileMovementComponent 설정
    ProjectileMovement->Velocity = LaunchDirection * LaunchSpeed;
    ProjectileMovement->InitialSpeed = LaunchSpeed;
    ProjectileMovement->MaxSpeed = LaunchSpeed * 1.5f;
    
    // 호밍 설정 (정확한 도착을 위해)
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingTargetComponent = TargetCharacter->GetRootComponent();
    ProjectileMovement->HomingAccelerationMagnitude = LaunchSpeed * 2.0f; // 호밍 강도
    
    // 활성화
    ProjectileMovement->Activate();
    
    UE_LOG(LogTemp, Warning, TEXT("Throwing with homing from: %s to: %s"), *StartLocation.ToString(), *TargetLocation.ToString());
    
    // 액터 참조 해제
    AActor* ThrownActor = ActorToThrow;
    ActorToThrow = nullptr;
    
    return ThrownActor;
}

bool USLBossAnimInstance::JumpToTargetPoint(AActor* TargetPointActor, bool bUpdateRotation, float RemainingAnimTime, float OffsetDistance)
{
    if (!TargetPointActor || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot jump to target point: Missing target point actor or owner"));
        return false;
    }
    bIsFalling = true;
    
    FVector StartLocation = OwningCharacter->GetActorLocation();
    FVector TargetLocation = TargetPointActor->GetActorLocation();
    
    // 착지 목표 위치 계산 (오프셋 적용)
    TargetLandingLocation = TargetLocation;
    
    // 오프셋이 설정된 경우, 타겟에서 랜덤한 방향으로 오프셋 적용
    if (OffsetDistance > 0.0f)
    {
        FVector RandomDirection = FVector(
            FMath::RandRange(-1.0f, 1.0f),
            FMath::RandRange(-1.0f, 1.0f),
            0.0f
        ).GetSafeNormal();
        
        TargetLandingLocation += RandomDirection * OffsetDistance;
    }
    
    // 땅 높이에 맞춰 착지 위치 조정
    FHitResult GroundHit;
    FVector TraceStart = TargetLandingLocation + FVector(0, 0, 500.0f);
    FVector TraceEnd = TargetLandingLocation + FVector(0, 0, -1000.0f);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwningCharacter);
    QueryParams.AddIgnoredActor(TargetPointActor);
    QueryParams.bTraceComplex = false;
    
    // 실제 땅 높이 찾기
    if (OwningCharacter->GetWorld()->LineTraceSingleByChannel(
        GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
    {
        TargetLandingLocation.Z = GroundHit.Location.Z;
    }
    
    // 점프 벡터 계산
    FVector JumpDirection = TargetLandingLocation - StartLocation;
    float HorizontalDistance = FVector::Dist2D(StartLocation, TargetLandingLocation);
    float VerticalDistance = TargetLandingLocation.Z - StartLocation.Z;
    
    // 점프 시간 계산 (간단한 방식)
    float JumpTime = 2.0f; // 기본 2초
    if (RemainingAnimTime > 0.5f)
    {
        JumpTime = RemainingAnimTime;
    }
    
    // 중력 가져오기
    float Gravity = FMath::Abs(GetWorld()->GetGravityZ());
    if (Gravity < 100.0f) Gravity = 980.0f; // 기본값
    
    // 수평 속도 계산
    FVector HorizontalDirection = FVector(JumpDirection.X, JumpDirection.Y, 0.0f).GetSafeNormal();
    float HorizontalSpeed = HorizontalDistance / JumpTime;
    
    // 수직 속도 계산 (포물선 운동)
    float VerticalSpeed = (VerticalDistance / JumpTime) + (0.5f * Gravity * JumpTime);
    
    // 최종 점프 속도
    FVector JumpVelocity = HorizontalDirection * HorizontalSpeed;
    JumpVelocity.Z = VerticalSpeed;
    
    // 속도 제한
    float MaxSpeed = 2500.0f;
    if (JumpVelocity.Size() > MaxSpeed)
    {
        JumpVelocity = JumpVelocity.GetSafeNormal() * MaxSpeed;
    }
    
    // 캐릭터 회전
    if (bUpdateRotation)
    {
        FRotator LookRotation = FRotationMatrix::MakeFromX(HorizontalDirection).Rotator();
        OwningCharacter->SetActorRotation(LookRotation);
    }
    
    // 충돌 설정 (점프 중 캐릭터 간 충돌 방지)
    SetupJumpCollision();
    
    // 점프 실행
    OwningCharacter->LaunchCharacter(JumpVelocity, true, true);
    
    // 착지 모니터링 시작
    StartLandingCheck();
    
    return true;
}

void USLBossAnimInstance::FinishJump()
{
    CompleteLanding();
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
    
    // 타겟 위치
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
    
    FVector ChargeVelocity = DirectionXY * ChargeSpeed;
    // Z 속도는 유지 (중력의 영향으로 떨어지도록)
    ChargeVelocity.Z = 0.0f;
    OwningCharacter->LaunchCharacter(ChargeVelocity, true, true);
    
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
}

void USLBossAnimInstance::SetupJumpCollision()
{
    if (!OwningCharacter) return;
    
    // 메시 충돌 설정 저장 및 변경
    if (UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh())
    {
        OriginalCollisionType = CharacterMesh->GetCollisionEnabled();
        CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    // 캡슐 충돌 설정
    if (UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent())
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
}

void USLBossAnimInstance::StartLandingCheck()
{
    if (!OwningCharacter) return;
    
    if (LandingCheckTimer.IsValid())
    {
        OwningCharacter->GetWorldTimerManager().ClearTimer(LandingCheckTimer);
    }
    
    OwningCharacter->GetWorldTimerManager().SetTimer(
        LandingCheckTimer,
        this,
        &USLBossAnimInstance::CheckForLanding,
        0.1f,
        true
    );
}

void USLBossAnimInstance::CheckForLanding()
{
    if (!OwningCharacter)
    {
        return;
    }
    
    float DistanceToGround = GetDistanceToGround();
    FVector CurrentVelocity = OwningCharacter->GetVelocity();
    float VerticalVelocity = CurrentVelocity.Z;
    
    bool bShouldLand = false;
    
    if (DistanceToGround <= 200.0f && VerticalVelocity <= 0.0f)
    {
        bShouldLand = true;
    }
    
    float DistanceToTarget = FVector::Dist(OwningCharacter->GetActorLocation(), TargetLandingLocation);
    if (DistanceToTarget <= 100.0f && VerticalVelocity <= 50.0f)
    {
        bShouldLand = true;
    }
    
    if (bShouldLand)
    {
        CompleteLanding();
    }
}

void USLBossAnimInstance::CompleteLanding()
{
    if (!OwningCharacter) return;
    
    bIsFalling = false;
    
    // 착지 체크 타이머 정리
    if (LandingCheckTimer.IsValid())
    {
        OwningCharacter->GetWorldTimerManager().ClearTimer(LandingCheckTimer);
    }
    
    // 충돌 설정 복원
    RestoreJumpCollision();
    
    // 착지 상태로 설정
    bIsFalling = false;
    
    // 애니메이션 속도 정상화
    if (IsAnyMontagePlaying())
    {
        if (FAnimMontageInstance* MontageInstance = GetActiveMontageInstance())
        {
            MontageInstance->SetPlayRate(1.0f);
        }
    }
}

void USLBossAnimInstance::RestoreJumpCollision()
{
    if (!OwningCharacter) return;
    
    // 메시 충돌 복원
    if (UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh())
    {
        CharacterMesh->SetCollisionEnabled(OriginalCollisionType);
    }
    
    // 캡슐 충돌 복원
    if (UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent())
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    }
}

void USLBossAnimInstance::CleanupJumpTimers()
{
    if (!OwningCharacter) return;
    
    // 기존 점프 타이머 정리
    if (JumpTimerHandle.IsValid())
    {
        OwningCharacter->GetWorldTimerManager().ClearTimer(JumpTimerHandle);
    }
    
    // 착지 체크 타이머 정리
    if (LandingCheckTimer.IsValid())
    {
        OwningCharacter->GetWorldTimerManager().ClearTimer(LandingCheckTimer);
    }
}