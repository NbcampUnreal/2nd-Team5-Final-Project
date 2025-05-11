// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMonster.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/SLMonsterAIController.h"

ASLMonster::ASLMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASLMonsterAIController::StaticClass();

}

void ASLMonster::BeginPlay()
{
	Super::BeginPlay();
	LastAttackTime = 0.0f;
	
}

void ASLMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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
    UE_LOG(LogTemp, Warning, TEXT("Monster attacks the player!"));
}

