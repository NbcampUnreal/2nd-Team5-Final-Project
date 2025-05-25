#include "BattleComponent.h"

#include "MotionWarpingComponent.h"

DEFINE_LOG_CATEGORY(LogBattleComponent);

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
				UE_LOG(LogBattleComponent, Log, TEXT("Send Hit Result: %s -> %s | Damage: %.1f | AnimType: %s"),
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
		UE_LOG(LogBattleComponent, Warning,
			   TEXT("피격 발생! 소유자: %s, 데미지: %.2f, 공격자: %s, 위치: %s, 뼈: %s, AnimType: %s"),
			   *OwnerActor->GetName(),
			   DamageAmount,
			   DamageCauser ? *DamageCauser->GetName() : TEXT("None"),
			   *HitResult.ImpactPoint.ToString(),
			   *HitResult.BoneName.ToString(),
			   *UEnum::GetValueAsString(AnimType)
		);
		
		OnCharacterHited.Broadcast(DamageCauser, DamageAmount, HitResult, AnimType);
	}
}

void UBattleComponent::DoAttackSweep(EAttackAnimType AttackType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		const FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 50);
		const FVector End = Start + OwnerActor->GetActorForwardVector() * 100;
		const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(30.f, 70.f);

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
					if (AttackType == EAttackAnimType::AAT_FinalAttackA
						|| AttackType == EAttackAnimType::AAT_FinalAttackB
						|| AttackType == EAttackAnimType::AAT_FinalAttackC)
					{
						FVector TargetLoc = HitActor->GetActorLocation() - HitActor->GetActorForwardVector()*137.f;
						FRotator TargetRot = HitActor->GetActorRotation();
						MotionWarpComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);

						UE_LOG(LogBattleComponent, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
						AlreadyHitActors.Add(HitActor);

						SendHitResult(HitActor, Hit, AttackType);
					}
					else
					{
						UE_LOG(LogBattleComponent, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
						AlreadyHitActors.Add(HitActor);

						SendHitResult(HitActor, Hit, AttackType);
					}
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