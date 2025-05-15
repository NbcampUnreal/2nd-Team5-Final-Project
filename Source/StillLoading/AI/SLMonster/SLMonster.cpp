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

}

void ASLMonster::BeginPlay()
{
	Super::BeginPlay();
	LastAttackTime = 0.0f;
    GetCharacterMovement()->MaxWalkSpeed = 200.f;
	
}

void ASLMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AActor* ClosestEnemy = nullptr;
    float ClosestDistance = MAX_FLT;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* OtherPawn = *It;
        if (OtherPawn == this) continue;

        AController* OtherController = OtherPawn->GetController();
        if (!OtherController) continue;

        IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherController);
        if (!OtherTeamAgent) continue;

        ASLEnemyAIController* MyController = Cast<ASLEnemyAIController>(GetController());
        if (!MyController) continue;

        ETeamAttitude::Type Attitude = MyController->GetTeamAttitudeTowards(*OtherPawn);
        if (Attitude != ETeamAttitude::Hostile) continue;

        float Distance = FVector::Dist(GetActorLocation(), OtherPawn->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestEnemy = OtherPawn;
            ClosestDistance = Distance;
        }
    }

    if (ClosestEnemy)
    {
        TargetActor = ClosestEnemy;

        float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
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
    TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
    AnimInstance->SetIsAttacking(true);
}



