// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
#include "Components/CapsuleComponent.h"
#include "Controller/SLEnemyAIController.h"
#include "Components/SphereComponent.h"

ASLMonster::ASLMonster()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->bUseRVOAvoidance = true;
        MovementComponent->AvoidanceConsiderationRadius = AvoidanceRadius;
        MovementComponent->AvoidanceWeight = 0.5f;
    }

    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(GetMesh(), "WeaponSocket");

    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->SetSphereRadius(550.f);
    AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttackSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    AttackSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
    
    AttackCheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCheckSphere"));
    AttackCheckSphere->SetupAttachment(RootComponent);
    AttackCheckSphere->SetSphereRadius(550.f);
    AttackCheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackCheckSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    AttackCheckSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

    // BattleComponent 에서 사용 하기위한 캡슐 셋팅
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
}

void ASLMonster::BeginPlay()
{
    Super::BeginPlay();
    LastAttackTime = 0.0f;
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;

    ASLEnemyAIController* EnemyController = Cast<ASLEnemyAIController>(GetController());
    if (EnemyController)
    {
        BlackboardComp = EnemyController->GetBlackboardComponent();
    }
}

void ASLMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ASLEnemyAIController* EnemyController = Cast<ASLEnemyAIController>(GetController());
    if (EnemyController->bPatrolPointsReady)
    {
        CurrentTargetLocation = BlackboardComp->GetValueAsVector("PatrolLocation");
        PatrolPoints = EnemyController->PatrolPoints;
        EnemyController->bPatrolPointsReady = false;
        
    }
    if (PatrolPoints.Num() > 0 && IsCloseToTargetPoint())
    {
        MoveToNextPatrolPoint();
    }

    RandCoolTime = FMath::FRandRange(1.5f, 2.5f);
    BlackboardComp->SetValueAsFloat(FName("RandCoolTime"), RandCoolTime);
}

void ASLMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLMonster::Attack()
{
    AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("TargetActor")));
    if (!Target) return;
    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    if (Distance > AttackRange) return;
    /*{
        FVector RetreatDirection = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
        FVector RetreatLocation = GetActorLocation() + RetreatDirection * 350.f;

        AAIController* EnemyCon = Cast<AAIController>(GetController());
        EnemyCon->MoveToLocation(RetreatLocation);
    }*/
    AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (IsTargetInSight())
    {
        if (CurrentTime - LastAttackTime > AttackCooldown)
        {
            RandomAttackInt = FMath::RandRange(0, 1);
            TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
            AnimInstance->SetIsAttacking(true);
            LastAttackTime = CurrentTime;
        }
    }
}

void ASLMonster::SetPatrolPoints(const TArray<FVector>& Points)
{
    PatrolPoints = Points;

    if (PatrolPoints.Num() > 0)
    {
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsVector("PatrolLocation", CurrentTargetLocation);
        }
    }
}

bool ASLMonster::IsCloseToTargetPoint(float AcceptableDistance)
{
    float Distance = FVector::Dist(GetActorLocation(), CurrentTargetLocation);
    return Distance <= AcceptableDistance;
}

void ASLMonster::MoveToNextPatrolPoint()
{
    if (PatrolPoints.Num() == 0) return;
    
    CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
    CurrentTargetLocation = PatrolPoints[CurrentPatrolIndex];

    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector("PatrolLocation", CurrentTargetLocation);
    }
}

void ASLMonster::SetRVOAvoidanceEnabled(bool Enable)
{
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->bUseRVOAvoidance = Enable;
    }
}

bool ASLMonster::IsTargetInSight() const
{
    AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("TargetActor")));
    if (!Target) return false;

    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0.f;
    ToTarget.Normalize();

    FVector Forward = GetActorForwardVector();
    Forward.Z = 0.f;
    Forward.Normalize();

    float Dot = FVector::DotProduct(Forward, ToTarget);
    float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(AttackAngle * 0.5f));

    return Dot >= CosHalfFOV;
    /*AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("TargetActor")));
    if (!Target) return false;

    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Normalize();

    FVector Forward = GetActorForwardVector();
    float Dot = FVector::DotProduct(Forward, ToTarget);

    float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(AttackAngle * 0.5f));
    return Dot >= CosHalfFOV;*/
}

void ASLMonster::SetIsInCombat(bool bInCombat)
{
    bIsCombat = bInCombat;
}

bool ASLMonster::GetIsInCombat() const
{
    return bIsCombat;
}



