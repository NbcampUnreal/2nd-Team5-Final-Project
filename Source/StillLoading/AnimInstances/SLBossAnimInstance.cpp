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

AActor* USLBossAnimInstance::ThrowActorAtTarget(float LaunchSpeed, float ArcParam, FName SocketName)
{
    if (!ActorToThrow || !TargetCharacter || !OwningCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot throw actor: Missing actor to throw or target"));
        return nullptr;
    }
    
    // 액터 디태치 - 발사 전에 손에서 분리
    USceneComponent* RootComp = ActorToThrow->GetRootComponent();
    if (RootComp)
    {
        RootComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }
    
    // 던질 액터의 현재 위치
    FVector StartLocation = OwningCharacter->GetMesh()->GetSocketLocation(SocketName);
    
    // 액터가 분리됐으므로 시작 위치 설정 
    ActorToThrow->SetActorLocation(StartLocation);
    
    // 타겟 위치
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    
    // 프로젝타일 무브먼트 컴포넌트 추가
    UProjectileMovementComponent* ProjectileMovement = ActorToThrow->FindComponentByClass<UProjectileMovementComponent>();
    
    if (!ProjectileMovement)
    {
        ProjectileMovement = NewObject<UProjectileMovementComponent>(ActorToThrow, UProjectileMovementComponent::StaticClass());
        ProjectileMovement->RegisterComponent();
    }
    
    // 프로젝타일 무브먼트 설정
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 1.0f;
    ProjectileMovement->Velocity = FVector::ZeroVector;
    ProjectileMovement->InitialSpeed = LaunchSpeed;
    ProjectileMovement->MaxSpeed = LaunchSpeed;
    
    // 물리 활성화
    UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(ActorToThrow->GetRootComponent());
    if (PrimComp)
    {
        PrimComp->SetSimulatePhysics(true);
        PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    
    // 발사 방향 및 속도 계산
    FVector ToTarget = TargetLocation - StartLocation;
    FVector DirectionXY = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
    
    // 포물선 궤적을 위한 Z 성분 조정
    float DistanceXY = FVector::Dist2D(StartLocation, TargetLocation);
    float LaunchAngle = FMath::DegreesToRadians(45.0f * ArcParam); // ArcParam으로 궤적 높이 조절
    
    // 수직 속도 계산
    float Vz = LaunchSpeed * FMath::Sin(LaunchAngle);
    
    // 최종 발사 속도
    FVector LaunchVelocity = DirectionXY * LaunchSpeed * FMath::Cos(LaunchAngle);
    LaunchVelocity.Z = Vz;
    
    // 발사
    ProjectileMovement->Velocity = LaunchVelocity;
    
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
    
    // 남은 애니메이션 시간 자동 계산
    float EstimatedJumpTime = 1.0f;
    
    if (RemainingAnimTime <= 0.0f)
    {
        // 현재 재생 중인 몽타주가 있으면 해당 몽타주의 남은 시간 사용
        if (IsAnyMontagePlaying())
        {
            FAnimMontageInstance* MontageInstance = GetActiveMontageInstance();
            if (MontageInstance && MontageInstance->Montage)
            {
                float CurrentTime = MontageInstance->GetPosition();
                float TotalDuration = MontageInstance->Montage->GetPlayLength();
                EstimatedJumpTime = TotalDuration - CurrentTime;
            }
        }
    }
    else
    {
        EstimatedJumpTime = RemainingAnimTime;
    }
    
    // 최소 시간 보장
    if (EstimatedJumpTime < 0.2f)
    {
        EstimatedJumpTime = 0.2f;
    }
    
    // 출발 위치
    FVector StartLocation = OwningCharacter->GetActorLocation();
    
    // 타겟 위치
    FVector TargetLocation = TargetCharacter->GetActorLocation();
    FVector TargetForward = TargetCharacter->GetActorForwardVector();
    TargetLocation -= TargetForward * 150.0f; // 타겟 앞 150cm에 착지 (오버슈팅 방지를 위해 증가)
    
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
    float Gravity = GetWorld()->GetGravityZ() * -1.0f;
    if (OwningMovementComponent)
    {
        Gravity = OwningMovementComponent->GetGravityZ() * -1.0f;
    }
   
    // 75%에서 85%로 변경 (더 많은 애니메이션 시간을 이동에 사용)
    float TimeToReachTarget = EstimatedJumpTime * 0.85f;
    
    // 감속 계수 적용 (오버슈팅 방지)
    float SpeedReductionFactor = 0.9f; // 10% 감속
    
    // 주어진 시간과 거리로 필요한 수평 속도 계산
    float RequiredHorizontalSpeed = (DistanceXY / TimeToReachTarget) * SpeedReductionFactor;

    float HeightDifference = TargetLocation.Z - StartLocation.Z;
    // 중력을 고려한 수직 속도 계산
    float Vz = (HeightDifference / TimeToReachTarget) + (0.5f * Gravity * TimeToReachTarget);
    
    // 최종 속도 벡터 계산
    FVector JumpVelocity = DirectionXY * RequiredHorizontalSpeed;
    JumpVelocity.Z = Vz;
    
    // 최소/최대 속도 제한 (필요시 조정) - 최대 속도를 낮춤
    float MaxSpeed = 1500.0f; // 2000에서 1500으로 낮춤
    float MinSpeed = 300.0f;
    
    float CurrentSpeed = JumpVelocity.Size();
    if (CurrentSpeed > MaxSpeed)
    {
        JumpVelocity = JumpVelocity.GetSafeNormal() * MaxSpeed;
    }
    else if (CurrentSpeed < MinSpeed)
    {
        JumpVelocity = JumpVelocity.GetSafeNormal() * MinSpeed;
    }
    
    // 캐릭터 충돌 비활성화
    UPrimitiveComponent* CharacterMesh = OwningCharacter->GetMesh();
    if (CharacterMesh)
    {
        // 현재 충돌 설정 저장
        OriginalCollisionType = CharacterMesh->GetCollisionEnabled();
        
        // 충돌 비활성화
        CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    
    // 캡슐 컴포넌트 충돌 설정
    UCapsuleComponent* CapsuleComp = OwningCharacter->GetCapsuleComponent();
    if (CapsuleComp)
    {
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
    
    // 점프 적용
    OwningCharacter->LaunchCharacter(JumpVelocity, true, true);
    
    // 타이머를 설정하여 점프 종료 시 충돌 재활성화
    OwningCharacter->GetWorldTimerManager().SetTimer(
        JumpTimerHandle,
        this,
        &USLBossAnimInstance::FinishJump,
        EstimatedJumpTime + 0.2f,
        false
    );
    
    return true;
}

void USLBossAnimInstance::FinishJump()
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
        
        // 타이머 초기화
        if (JumpTimerHandle.IsValid())
        {
            OwningCharacter->GetWorldTimerManager().ClearTimer(JumpTimerHandle);
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





