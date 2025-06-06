#include "SLBattlePlayerState.h"

#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ASLBattlePlayerState::ASLBattlePlayerState()
{
	bReplicates = true;

	Health = MaxHealth;
	bIsWalking = false;
	BurningGage = 0;
}

void ASLBattlePlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBattlePlayerState::SetHealth(const float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	OnRep_Health();
}

void ASLBattlePlayerState::DecreaseHealth(const float Amount)
{
	Health = FMath::Clamp(Health - Amount, 0.f, Health);
	OnRep_Health();
}

void ASLBattlePlayerState::SetWalking(const bool bNewWalking)
{
	bIsWalking = bNewWalking;
	OnRep_IsWalking();
}

void ASLBattlePlayerState::SetMaxSpeed(const float NewMaxSpeed)
{
	MaxSpeed = NewMaxSpeed;
	if (APawn* OwnerPawn = GetPawn())
	{
		if (auto* Movement = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>())
		{
			Movement->MaxWalkSpeed = MaxSpeed;
		}
	}
}

void ASLBattlePlayerState::IncreaseBurningGage(const float Amount)
{
	if (const AController* OwnerController = GetOwner<AController>())
	{
		if (const APawn* Pawn = OwnerController->GetPawn())
		{
			if (const UCombatHandlerComponent* CombatHandler = Pawn->FindComponentByClass<UCombatHandlerComponent>())
			{
				if (CombatHandler->IsEmpowered())
				{
					return;
				}
				else
				{
					if (BurningGage >= 100)
					{
						BurningGage = 0;
		
						if (UMovementHandlerComponent* MoveComp = Pawn->FindComponentByClass<UMovementHandlerComponent>())
						{
							MoveComp->BeginBuff();
						}
					}
				}
			}
		}
	}
	
	BurningGage += Amount;
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
			Movement->MaxWalkSpeed = bIsWalking ? WalkingSpeed : MaxSpeed;
		}
	}
}

void ASLBattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASLBattlePlayerState, Health);
	DOREPLIFETIME(ASLBattlePlayerState, bIsWalking);
}