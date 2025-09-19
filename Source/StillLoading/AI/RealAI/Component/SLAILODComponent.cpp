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

FString USLAILODComponent::LODLevelToString(EAILODLevel Level)
{
	switch (Level)
	{
	case EAILODLevel::Max:    return TEXT("Max");
	case EAILODLevel::High:   return TEXT("High");
	case EAILODLevel::Medium: return TEXT("Medium");
	case EAILODLevel::Low:    return TEXT("Low");
	case EAILODLevel::Culled: return TEXT("Culled");
	default:                  return TEXT("Unknown");
	}
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
	
    if (CurrentLODLevel == NewLevel)
    {
        return;
    }
	
    CurrentLODLevel = NewLevel;

    USLAIStateComponent* CurrentStateComponent = OwnerCharacter->AIStateComp;
    USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
    UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
    AController* Controller = OwnerCharacter->GetController();

    if (!Mesh || !MovementComponent || !Capsule) return;
	
    switch (CurrentLODLevel)
    {
    case EAILODLevel::Max:
    case EAILODLevel::High:
    	OwnerCharacter->SetActorHiddenInGame(false);
    	OwnerCharacter->ToggleWeaponState(true);
    	OwnerCharacter->ActivateMovementComponent();

        OwnerCharacter->SetActorTickEnabled(true);
        if (Controller) Controller->SetActorTickEnabled(true);

        Mesh->SetComponentTickEnabled(true);
        Mesh->SetVisibility(true);
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

        MovementComponent->SetComponentTickEnabled(true);
        MovementComponent->SetAvoidanceEnabled(true);
        Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        if (CurrentStateComponent)
        {
            CurrentStateComponent->SetComponentTickEnabled(true);
            GetWorld()->GetTimerManager().UnPauseTimer(CurrentStateComponent->DetectionTimerHandle);
        }

        if (CurrentLODLevel == EAILODLevel::Max)
        {
            Mesh->SetCastShadow(true);
            if (CurrentStateComponent) CurrentStateComponent->SetComponentTickInterval(0.1f);
        }
        else
        {
            Mesh->SetCastShadow(false);
            if (CurrentStateComponent) CurrentStateComponent->SetComponentTickInterval(0.2f);
        }
        break;

    case EAILODLevel::Medium:
    	OwnerCharacter->SetActorHiddenInGame(false);
    	OwnerCharacter->ToggleWeaponState(true);
    	OwnerCharacter->ActivateMovementComponent();
        
    	OwnerCharacter->SetActorTickEnabled(false);
    	if (Controller) Controller->SetActorTickEnabled(false);
       
    	Mesh->SetComponentTickEnabled(false);
    	Mesh->SetCastShadow(false);
    	Mesh->SetVisibility(true);
    	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

    	MovementComponent->SetComponentTickEnabled(false);

    	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    	if (CurrentStateComponent)
    	{
    		CurrentStateComponent->SetComponentTickEnabled(true);
    		CurrentStateComponent->SetComponentTickInterval(1.0f);

    		GetWorld()->GetTimerManager().PauseTimer(CurrentStateComponent->DetectionTimerHandle);
    	}
        break;
    case EAILODLevel::Low:
    case EAILODLevel::Culled:
    	OwnerCharacter->SetActorHiddenInGame(true);
    	OwnerCharacter->ToggleWeaponState(false);
        
        OwnerCharacter->SetActorTickEnabled(false);
        if (Controller) Controller->SetActorTickEnabled(false);
        
        Mesh->SetComponentTickEnabled(false);
        Mesh->SetCastShadow(false);
        Mesh->SetSimulatePhysics(false); 
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

        MovementComponent->SetComponentTickEnabled(false);
        MovementComponent->Deactivate();
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    	//Capsule->SetMobility(EComponentMobility::Static);

        if (CurrentStateComponent)
        {
        	CurrentStateComponent->SetState(EAIBattleState::Idle);
            CurrentStateComponent->SetComponentTickEnabled(false);
            GetWorld()->GetTimerManager().PauseTimer(CurrentStateComponent->DetectionTimerHandle);
        }
        break;
    }
}
