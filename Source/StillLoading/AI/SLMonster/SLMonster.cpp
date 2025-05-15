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
    GetCharacterMovement()->MaxWalkSpeed = 350.0f;
}

void ASLMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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

//void ASLMonster::SetRVOAvoidanceEnabled(bool Enable)
//{
//    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
//    if (MovementComponent)
//    {
//        MovementComponent->bUseRVOAvoidance = true;
//    }
//}



