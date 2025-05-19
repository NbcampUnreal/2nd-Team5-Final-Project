#include "BattleComponent.h"

UBattleComponent::UBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBattleComponent::SendHitResult_Implementation(AActor* HitTarget, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (HitTarget && HitTarget->GetClass()->ImplementsInterface(USLBattleInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Send Hit Result: %s -> %s | Damage: %.1f | AnimType: %s"),
				*OwnerActor->GetName(),
				*HitTarget->GetName(),
				DamageAmount,
				*UEnum::GetValueAsString(AnimType));
			
			ISLBattleInterface::Execute_ReceiveHitResult(HitTarget, DamageAmount, OwnerActor, HitResult, AnimType);
		}
	}
}

void UBattleComponent::ReceiveHitResult_Implementation(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	if (const AActor* OwnerActor = GetOwner())
	{
		OnCharacterHited.Broadcast(DamageCauser, DamageAmount, HitResult, AnimType);
	}
}