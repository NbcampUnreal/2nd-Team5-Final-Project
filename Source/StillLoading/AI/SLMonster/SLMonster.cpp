// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
#include "Controller/SLEnemyAIController.h"


ASLMonster::ASLMonster()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    /*UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->bUseRVOAvoidance = true;
        MovementComponent->AvoidanceConsiderationRadius = AvoidanceRadius;
        MovementComponent->AvoidanceWeight = 0.5f;
    }*/

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
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime > AttackCooldown)
    {
        TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
        AnimInstance->SetIsAttacking(true);
        LastAttackTime = CurrentTime;
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

//void ASLMonster::SetRVOAvoidanceEnabled(bool Enable)
//{
//    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
//    if (MovementComponent)
//    {
//        MovementComponent->bUseRVOAvoidance = true;
//    }
//}



