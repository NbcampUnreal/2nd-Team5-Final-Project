
#include "AI/NPC/SLNPC.h"
#include "Controller/SLNPCAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "EngineUtils.h"

ASLNPC::ASLNPC()
{
	PrimaryActorTick.bCanEverTick = false;
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
	GetCharacterMovement()->MaxWalkSpeed = 650.0f;
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

//void ASLNPC::SetRVOAvoidanceEnabled(bool Enable)
//{
//    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
//    if (MovementComponent)
//    {
//        MovementComponent->bUseRVOAvoidance = true;
//    }
//}

