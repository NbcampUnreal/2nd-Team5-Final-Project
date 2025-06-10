#include "SLBattlePlayerState.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/Item/SLDefaultSword.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ASLBattlePlayerState::ASLBattlePlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Health = MaxHealth;
	bIsWalking = false;
	BurningGage = 0;
}

void ASLBattlePlayerState::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
	   GaugeUpdateTimerHandle,
	   this,
	   &ASLBattlePlayerState::UpdateGauge,
	   5.0f,
	   true
   );
}

void ASLBattlePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(GaugeUpdateTimerHandle);
}

void ASLBattlePlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
	if (!HasAuthority()) return;
	
	const AController* OwnerController = GetOwner<AController>();
	if (!OwnerController) return;

	const APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn) return;

	const UCombatHandlerComponent* CombatHandler = Pawn->FindComponentByClass<UCombatHandlerComponent>();
	if (!CombatHandler || CombatHandler->IsEmpowered()) return;

	BurningGage += Amount;

	if (BurningGage >= 100.f)
	{
		BurningGage = 0.f;

		if (UMovementHandlerComponent* MoveComp = Pawn->FindComponentByClass<UMovementHandlerComponent>())
		{
			MoveComp->BeginBuff();
		}
	}

	// 싱글에선 직접 호출
	OnRep_BurningGage();
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

void ASLBattlePlayerState::OnRep_BurningGage()
{
	const AController* OwnerController = GetOwner<AController>();
	if (!OwnerController) return;

	const APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn) return;

	const ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(Pawn);
	if (!PlayerCharacter) return;

	if (ASLDefaultSword* SwordActor = Cast<ASLDefaultSword>(PlayerCharacter->Sword))
	{
		SwordActor->UpdateMaterialByGauge(BurningGage);
	}
}

void ASLBattlePlayerState::UpdateGauge()
{
	if (BurningGage <= 0) return;
	BurningGage = FMath::Clamp(BurningGage - 1.f, 0.f, 100.f);
	UE_LOG(LogTemp, Warning, TEXT("UpdateGauge [%f]"), BurningGage);
	// 싱글에선 직접 호출
	OnRep_BurningGage();
}

void ASLBattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASLBattlePlayerState, Health);
	DOREPLIFETIME(ASLBattlePlayerState, bIsWalking);
	DOREPLIFETIME(ASLBattlePlayerState, BurningGage);
}
