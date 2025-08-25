#include "SLAILODComponent.h"

#include "SLAIStateComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USLAILODComponent::USLAILODComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.3f;
}

void USLAILODComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AILODComponent is attached to a non-character actor. Disabling component."));
		SetComponentTickEnabled(false);
		return;
	}

	StateComponent = OwnerCharacter->FindComponentByClass<USLAIStateComponent>();
	SetLODLevel(CalculateLODLevel());
}

void USLAILODComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerCharacter) return;

	const EAILODLevel NewLODLevel = CalculateLODLevel();
	if (NewLODLevel != CurrentLODLevel)
	{
		SetLODLevel(NewLODLevel);
	}
}

EAILODLevel USLAILODComponent::CalculateLODLevel() const
{
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !OwnerCharacter) return EAILODLevel::Culled;

	const float DistanceSquared = FVector::DistSquared(OwnerCharacter->GetActorLocation(), PlayerPawn->GetActorLocation());
	EAILODLevel DistanceLOD;
    
	if (DistanceSquared <= FMath::Square(HighDetailDistance))
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

	switch (CurrentLODLevel)
	{
	case EAILODLevel::High:
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(true);
			StateComponent->SetComponentTickInterval(1.0f);
		}
		break;

	case EAILODLevel::Medium:
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(true);
			StateComponent->SetComponentTickInterval(2.0f);
		}
		break;

	case EAILODLevel::Low:
	case EAILODLevel::Culled:
		OwnerCharacter->SetActorHiddenInGame(true);
		OwnerCharacter->ToggleWeaponState(false);
		if (StateComponent)
		{
			StateComponent->SetComponentTickEnabled(false);
		}
		break;
	}
}
