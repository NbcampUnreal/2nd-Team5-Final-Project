
#include "AI/NPC/SLNPC.h"
#include "Controller/SLNPCAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"

ASLNPC::ASLNPC()
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

void ASLNPC::BeginPlay()
{
	Super::BeginPlay();
	LastAttackTime = 0.0f;
	GetCharacterMovement()->MaxWalkSpeed = 250.0f;

    ASLNPCAIController* NPCController = Cast<ASLNPCAIController>(GetController());
    if (NPCController)
    {
        BlackboardComp = NPCController->GetBlackboardComponent();
    }
}

void ASLNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ASLNPCAIController* NPCController = Cast<ASLNPCAIController>(GetController());
    if (NPCController->bPatrolPointsReady)
    {
        CurrentTargetLocation = BlackboardComp->GetValueAsVector("PatrolLocation");
        PatrolPoints = NPCController->PatrolPoints;
        NPCController->bPatrolPointsReady = false;
    }

    if (PatrolPoints.Num() > 0 && IsCloseToTargetPoint())
    {
        MoveToNextPatrolPoint();
    }
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
        //AnimInstance->SetIsAttacking(true);
        LastAttackTime = CurrentTime;
    }
}

void ASLNPC::SetPatrolPoints(const TArray<FVector>& Points)
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

bool ASLNPC::IsCloseToTargetPoint(float AcceptableDistance)
{
    float Distance = FVector::Dist(GetActorLocation(), CurrentTargetLocation);
    return Distance <= AcceptableDistance;
}

void ASLNPC::MoveToNextPatrolPoint()
{
    
    if (PatrolPoints.Num() == 0) return;
    
    CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
    CurrentTargetLocation = PatrolPoints[CurrentPatrolIndex];

    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsVector("PatrolLocation", CurrentTargetLocation);
    }
}

//void ASLNPC::SetRVOAvoidanceEnabled(bool Enable)
//{
//    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
//    if (MovementComponent)
//    {
//        MovementComponent->bUseRVOAvoidance = true;
//    }
//}

