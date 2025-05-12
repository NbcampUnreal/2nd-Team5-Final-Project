// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"

ASLMonster::ASLMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void ASLMonster::BeginPlay()
{
	Super::BeginPlay();
	LastAttackTime = 0.0f;
    GetCharacterMovement()->MaxWalkSpeed = 650.f;
	
}

void ASLMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /*APawn* ClosestEnemy = nullptr;
    float ClosestDistSq = FLT_MAX;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* TestPawn = *It;

        if (!TestPawn || TestPawn == this)
            continue;

        IGenericTeamAgentInterface* TeamAgent
    }*/


    if (!TargetPlayer)
        TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (TargetPlayer)
    {
        float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
        if (Distance < AttackRange)
        {
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastAttackTime > AttackCooldown)
            {
                Attack();
                LastAttackTime = CurrentTime;
            }
        }
    }
}

void ASLMonster::Attack()
{
    UE_LOG(LogTemp, Warning, TEXT("몬스터가 공격을 시작했습니다."));
    TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
    AnimInstance->SetIsAttacking(true);
}

