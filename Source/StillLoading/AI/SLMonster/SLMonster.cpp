// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
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

    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->SetSphereRadius(550.f);
    AttackSphere->SetCollisionProfileName(TEXT("OverlapAll"));
    AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AttackCheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCheckSphere"));
    AttackCheckSphere->SetupAttachment(RootComponent);
    AttackCheckSphere->SetSphereRadius(550.f);
    AttackCheckSphere->SetCollisionProfileName(TEXT("OverlapAll"));
    AttackCheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
    if (IsTargetInSight())
    {
        AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - LastAttackTime > AttackCooldown)
        {
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
    ToTarget.Normalize();

    FVector Forward = GetActorForwardVector();
    float Dot = FVector::DotProduct(Forward, ToTarget);

    float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(AttackAngle * 0.5f));
    return Dot >= CosHalfFOV;
}

void ASLMonster::SetIsInCombat(bool bInCombat)
{
    bIsCombat = bInCombat;
}

bool ASLMonster::GetIsInCombat() const
{
    return bIsCombat;
}



