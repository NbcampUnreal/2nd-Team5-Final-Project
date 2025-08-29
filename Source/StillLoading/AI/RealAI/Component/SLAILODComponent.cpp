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
}

EAILODLevel USLAILODComponent::CalculateLODLevel(float MaxDetailDistance, float HighDetailDistance, float MediumDetailDistance, float LowDetailDistance) const
{
	const ASLMonsterAICharacter* OwnerCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
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
	ASLMonsterAICharacter* OwnerCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!OwnerCharacter) return;

	USLAIStateComponent* CurrentStateComponent = GetOwner()->FindComponentByClass<USLAIStateComponent>();
	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
	UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
	AController* Controller = OwnerCharacter->GetController();
    
	if (CurrentLODLevel == NewLevel)
	{
		return;
	}
    
	CurrentLODLevel = NewLevel;

	if (!Mesh || !MovementComponent || !Capsule) return;

    if (CurrentLODLevel >= EAILODLevel::Low && NewLevel < EAILODLevel::Low)
    {
        OwnerCharacter->SetActorHiddenInGame(false);
        OwnerCharacter->ToggleWeaponState(true);
        OwnerCharacter->ActivateMovementComponent();
        OwnerCharacter->SetActorTickEnabled(true);
        if (Controller) Controller->SetActorTickEnabled(true);
        if (CurrentStateComponent) GetWorld()->GetTimerManager().UnPauseTimer(CurrentStateComponent->DetectionTimerHandle);
    }
    else if (CurrentLODLevel < EAILODLevel::Low && NewLevel >= EAILODLevel::Low)
    {
        OwnerCharacter->SetActorHiddenInGame(true);
        OwnerCharacter->ToggleWeaponState(false);
    }

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

        if (CurrentStateComponent)
        {
            if (CurrentStateComponent->GetCurrentState() == EAIBattleState::FakeMoving)
            {
                CurrentStateComponent->SetState(EAIBattleState::Idle);
            }
            CurrentStateComponent->SetComponentTickEnabled(true);
            CurrentStateComponent->SetComponentTickInterval(0.5f);
            GetWorld()->GetTimerManager().UnPauseTimer(CurrentStateComponent->DetectionTimerHandle);
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

        if (CurrentStateComponent)
        {
            if (CurrentStateComponent->GetCurrentState() == EAIBattleState::FakeMoving)
            {
                CurrentStateComponent->SetState(EAIBattleState::Idle);
            }
            CurrentStateComponent->SetComponentTickEnabled(true);
            CurrentStateComponent->SetComponentTickInterval(0.8f);
            GetWorld()->GetTimerManager().UnPauseTimer(CurrentStateComponent->DetectionTimerHandle);
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

        if (CurrentStateComponent)
        {
            CurrentStateComponent->SetComponentTickEnabled(true);
            CurrentStateComponent->SetComponentTickInterval(1.0f);
            CurrentStateComponent->SetState(EAIBattleState::FakeMoving);
            GetWorld()->GetTimerManager().PauseTimer(CurrentStateComponent->DetectionTimerHandle);
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

        if (CurrentStateComponent)
        {
            CurrentStateComponent->SetComponentTickEnabled(false);
            GetWorld()->GetTimerManager().PauseTimer(CurrentStateComponent->DetectionTimerHandle);
        }
        break;
    }
}
