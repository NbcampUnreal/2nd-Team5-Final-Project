#include "SLAILODComponent.h"

#include "SLAIStateComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "GameFramework/Character.h"
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
	SetLODLevel(CalculateLODLevel());
}

EAILODLevel USLAILODComponent::CalculateLODLevel() const
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

	if (CurrentLODLevel >= EAILODLevel::Low && NewLevel < EAILODLevel::Low)
	{
		OwnerCharacter->SetActorHiddenInGame(false);
		OwnerCharacter->ToggleWeaponState(true);
	}

	CurrentLODLevel = NewLevel;

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh) return;
	
	switch (CurrentLODLevel)
	{
	case EAILODLevel::Max:
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(true);
			StateComponent->SetComponentTickInterval(0.2f);
			Mesh->SetComponentTickEnabled(true);
		}
		break;
	case EAILODLevel::High:
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(true);
			StateComponent->SetComponentTickInterval(0.5f);
			Mesh->SetComponentTickEnabled(true);
		}
		break;
	case EAILODLevel::Medium:
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(true);
			StateComponent->SetComponentTickInterval(2.0f);
			Mesh->SetComponentTickEnabled(false);
		}
		break;
	case EAILODLevel::Low:
	case EAILODLevel::Culled:
		OwnerCharacter->SetActorHiddenInGame(true);
		OwnerCharacter->ToggleWeaponState(false);
		Mesh->SetComponentTickEnabled(false);
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(false);
		}
		break;
	}
}
