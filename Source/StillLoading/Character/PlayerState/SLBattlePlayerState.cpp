#include "SLBattlePlayerState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ASLBattlePlayerState::ASLBattlePlayerState()
{
	bReplicates = true;

	Health = MaxHealth;
	bIsWalking = false;
}

void ASLBattlePlayerState::SetHealth(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	OnRep_Health();
}

void ASLBattlePlayerState::SetWalking(const bool bNewWalking)
{
	bIsWalking = bNewWalking;
	OnRep_IsWalking();
}

void ASLBattlePlayerState::OnRep_Health()
{
	UE_LOG(LogTemp, Log, TEXT("Health Replicated: %.1f"), Health);
	// TODO: UI 연동, 사망처리 등
	
}

void ASLBattlePlayerState::OnRep_IsWalking()
{
	UE_LOG(LogTemp, Log, TEXT("Walking Toggled: %s"), bIsWalking ? TEXT("True") : TEXT("False"));

	if (APawn* OwnerPawn = GetPawn())
	{
		if (auto* Movement = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>())
		{
			Movement->MaxWalkSpeed = bIsWalking ? 200.f : 600.f;
		}
	}
}

void ASLBattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASLBattlePlayerState, Health);
	DOREPLIFETIME(ASLBattlePlayerState, bIsWalking);
}