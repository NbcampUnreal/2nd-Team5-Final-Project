#include "SLAILODComponent.h"

#include "SLAIStateComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

USLAILODComponent::USLAILODComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLAILODComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AILODComponent is attached to a non-character actor. Disabling component."));
		return;
	}

	StateComponent = OwnerCharacter->FindComponentByClass<USLAIStateComponent>();
}

EAILODLevel USLAILODComponent::CalculateLODLevel(float MaxDetailDistance, float HighDetailDistance, float MediumDetailDistance, float LowDetailDistance) const
{
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !OwnerCharacter) return EAILODLevel::Culled;

	const float DistanceSquared = FVector::DistSquared(OwnerCharacter->GetActorLocation(), PlayerPawn->GetActorLocation());
	EAILODLevel DistanceLOD;

	if (DistanceSquared <= FMath::Square(MaxDetailDistance))
	{
		DistanceLOD = EAILODLevel::Max;
	}
	else if (DistanceSquared <= FMath::Square(HighDetailDistance))
	{
		DistanceLOD = EAILODLevel::High;
	}
	else if (DistanceSquared <= FMath::Square(MediumDetailDistance))
	{
		DistanceLOD = EAILODLevel::Medium;
	}
	else if (DistanceSquared <= FMath::Square(LowDetailDistance))
	{
		DistanceLOD = EAILODLevel::Low;
	}
	else
	{
		DistanceLOD = EAILODLevel::Culled;
	}

	// 캐릭터의 메쉬가 최근 0.2초 내에 렌더링된 적이 있는지 확인.
	const bool bIsVisible = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->WasRecentlyRendered(0.2f) : false;

	if (!bIsVisible && DistanceLOD == EAILODLevel::High)
	{
		return EAILODLevel::Medium;
	}

	return DistanceLOD;
}

void USLAILODComponent::SetLODLevel(EAILODLevel NewLevel)
{
    if (CurrentLODLevel == NewLevel || !OwnerCharacter) return;

    USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
    UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
    AController* Controller = OwnerCharacter->GetController();

    if (!Mesh || !MovementComponent || !Capsule) return;

    if (CurrentLODLevel >= EAILODLevel::Low && NewLevel < EAILODLevel::Low)
    {
        OwnerCharacter->SetActorHiddenInGame(false);
        OwnerCharacter->ToggleWeaponState(true);
        OwnerCharacter->ActivateMovementComponent();
        OwnerCharacter->SetActorTickEnabled(true);
        if (Controller) Controller->SetActorTickEnabled(true);
        if (StateComponent) GetWorld()->GetTimerManager().UnPauseTimer(StateComponent->DetectionTimerHandle);
    }
    else if (CurrentLODLevel < EAILODLevel::Low && NewLevel >= EAILODLevel::Low)
    {
        OwnerCharacter->SetActorHiddenInGame(true);
        OwnerCharacter->ToggleWeaponState(false);
    }
    
    CurrentLODLevel = NewLevel;

    switch (CurrentLODLevel)
    {
    case EAILODLevel::Max:
        OwnerCharacter->SetActorTickEnabled(true);
        if (Controller) Controller->SetActorTickEnabled(true);
        
        Mesh->SetComponentTickEnabled(true);
        Mesh->SetCastShadow(true);
    	Mesh->SetVisibility(true);
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

        OwnerCharacter->ActivateMovementComponent();
        MovementComponent->SetComponentTickEnabled(true);
        MovementComponent->SetAvoidanceEnabled(true);
        
        Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        if (StateComponent)
        {
            if (StateComponent->GetCurrentState() == EAIBattleState::FakeMoving)
            {
                StateComponent->SetState(EAIBattleState::Idle);
            }
            StateComponent->SetComponentTickEnabled(true);
            StateComponent->SetComponentTickInterval(0.5f);
            GetWorld()->GetTimerManager().UnPauseTimer(StateComponent->DetectionTimerHandle);
        }
        break;

    case EAILODLevel::High:
        OwnerCharacter->SetActorTickEnabled(true);
        if (Controller) Controller->SetActorTickEnabled(true);

        Mesh->SetComponentTickEnabled(true);
        Mesh->SetCastShadow(false);
    	Mesh->SetVisibility(true);
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

        OwnerCharacter->ActivateMovementComponent();
        MovementComponent->SetComponentTickEnabled(true);
        MovementComponent->SetAvoidanceEnabled(true);

        Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        if (StateComponent)
        {
            if (StateComponent->GetCurrentState() == EAIBattleState::FakeMoving)
            {
                StateComponent->SetState(EAIBattleState::Idle);
            }
            StateComponent->SetComponentTickEnabled(true);
            StateComponent->SetComponentTickInterval(0.8f);
            GetWorld()->GetTimerManager().UnPauseTimer(StateComponent->DetectionTimerHandle);
        }
        break;

    case EAILODLevel::Medium:
        OwnerCharacter->SetActorTickEnabled(false);
        if (Controller) Controller->SetActorTickEnabled(false);
       
        Mesh->SetComponentTickEnabled(false);
        Mesh->SetCastShadow(false);
    	Mesh->SetVisibility(true);
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

        MovementComponent->SetComponentTickEnabled(false);
        MovementComponent->Deactivate();

        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        if (StateComponent)
        {
            StateComponent->SetComponentTickEnabled(true);
            StateComponent->SetComponentTickInterval(1.0f);
            StateComponent->SetState(EAIBattleState::FakeMoving);
            GetWorld()->GetTimerManager().PauseTimer(StateComponent->DetectionTimerHandle);
        }
        break;

    case EAILODLevel::Low:
    case EAILODLevel::Culled:
        OwnerCharacter->SetActorTickEnabled(false);
        if (Controller) Controller->SetActorTickEnabled(false);
        
        Mesh->SetComponentTickEnabled(false);
        Mesh->SetCastShadow(false);
    	Mesh->SetVisibility(false);
    	Mesh->SetSimulatePhysics(false); 
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

        MovementComponent->SetComponentTickEnabled(false);
        MovementComponent->Deactivate();
       
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        if (StateComponent)
        {
            StateComponent->SetComponentTickEnabled(false);
            GetWorld()->GetTimerManager().PauseTimer(StateComponent->DetectionTimerHandle);
        }
        break;
    }
}
