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

void UBattleComponent::DoAttackSweep()
{
	if (AActor* OwnerActor = GetOwner())
	{
		const FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 50);
		const FVector End = Start + OwnerActor->GetActorForwardVector() * 100;
		const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(40.f, 80.f);

		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerActor);

		GetWorld()->SweepMultiByChannel(
			HitResults,
			Start,
			End,
			FQuat::Identity,
			ECC_Pawn,
			SweepShape,
			Params
		);

		if (bShowDebugLine)
		{
			DrawDebugCapsule(GetWorld(), End, SweepShape.GetCapsuleHalfHeight(), SweepShape.GetCapsuleRadius(), FQuat::Identity, FColor::Green, false, 1.0f);
		}
		
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor && HitActor != OwnerActor && !AlreadyHitActors.Contains(HitActor))
			{
				if (HitActor->GetClass()->ImplementsInterface(USLBattleInterface::StaticClass()))
				{
					UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);
					ISLBattleInterface::Execute_SendHitResult(OwnerActor, HitActor, 10.0f, Hit, EAttackAnimType::AAT_NormalAttack);
				}
			}
		}
	}
}

void UBattleComponent::ClearHitTargets()
{
	AlreadyHitActors.Empty();
}