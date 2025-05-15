


#include "AI/NPC/SLNPC.h"
#include "Controller/SLNPCAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"

ASLNPC::ASLNPC()
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
}

void ASLNPC::BeginPlay()
{
	Super::BeginPlay();
	LastAttackTime = 0.0f;
	GetCharacterMovement()->MaxWalkSpeed = 650.0f;
	
}

void ASLNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    /*AActor* ClosestEnemy = nullptr;
    float ClosestDistance = MAX_FLT;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* OtherPawn = *It;
        if (OtherPawn == this) continue;

        AController* OtherController = OtherPawn->GetController();
        if (!OtherController) continue;

        IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherController);
        if (!OtherTeamAgent) continue;

        ASLNPCAIController* MyController = Cast<ASLNPCAIController>(GetController());
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
    }*/
}

void ASLNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASLNPC::Attack()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime > AttackCooldown)
    {
        TObjectPtr<USLAICharacterAnimInstance> AnimInstance = Cast<USLAICharacterAnimInstance>(GetMesh()->GetAnimInstance());
        AnimInstance->SetIsAttacking(true);
        LastAttackTime = CurrentTime;
    }
}

void ASLNPC::SetRVOAvoidanceEnabled(bool Enable)
{
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->bUseRVOAvoidance = true;
    }
}

