#include "BattleComponent.h"

UBattleComponent::UBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBattleComponent::SendHitResult(AActor* HitTarget, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (HitTarget)
		{
			if (UBattleComponent* TargetBattleComp = HitTarget->FindComponentByClass<UBattleComponent>())
			{
				UE_LOG(LogTemp, Warning, TEXT("Send Hit Result: %s -> %s | Damage: %.1f | AnimType: %s"),
					*OwnerActor->GetName(),
					*HitTarget->GetName(),
					GetDamageByType(AnimType),
					*UEnum::GetValueAsString(AnimType));

				TargetBattleComp->ReceiveHitResult(GetDamageByType(AnimType), OwnerActor, HitResult, AnimType);
			}
		}
	}
}

void UBattleComponent::ReceiveHitResult(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	if (const AActor* OwnerActor = GetOwner())
	{
		OnCharacterHited.Broadcast(DamageCauser, DamageAmount, HitResult, AnimType);
	}
}

void UBattleComponent::DoAttackSweep(EAttackAnimType AttackType)
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
			ECC_GameTraceChannel1,
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
				if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
				{
					UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);

					SendHitResult(HitActor, Hit, AttackType);
				}
			}
		}
	}
}

void UBattleComponent::ClearHitTargets()
{
	AlreadyHitActors.Empty();
}

float UBattleComponent::GetDamageByType(EAttackAnimType InType) const
{
	for (const auto& [AttackType, DamageAmount] : AttackData->AttackDataList)
	{
		if (AttackType == InType)
		{
			return DamageAmount;
		}
	}
	return 0.0f;
}