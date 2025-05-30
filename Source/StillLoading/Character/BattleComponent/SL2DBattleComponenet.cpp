// Fill out your copyright notice in the Description page of Project Settings.


#include "SL2DBattleComponenet.h"

#include "MotionWarpingComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

class UMotionWarpingComponent;

void USL2DBattleComponenet::DoAttackSweep(EAttackAnimType AttackType)
{
	if (ASLPlayerCharacter* OwnerActor = Cast<ASLPlayerCharacter>(GetOwner()))
	{
		const FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 25);
		const FVector End = Start + OwnerActor->GetMesh()->GetRightVector() * AttackOffset;
		const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(AttackSize, 70.f);

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
					UE_LOG(LogBattleComponent, Warning, TEXT("DoAttackSweep Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);

					SendHitResult(HitActor, Hit, AttackType);
				}
			}
		}
	}
}

void USL2DBattleComponenet::DoSweep(EAttackAnimType AttackType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		const FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 100);
		const FVector End = Start + OwnerActor->GetActorForwardVector() * 100;
		const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(100.f, 70.f);

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
						const FVector TargetForward = HitActor->GetActorForwardVector();
						const FVector TargetLoc = HitActor->GetActorLocation() + TargetForward * 120.f;

						const FVector OwnerLoc = OwnerActor->GetActorLocation();
						const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(TargetLoc, HitActor->GetActorLocation());

						if (UMotionWarpingComponent* WarpComp = Cast<UMotionWarpingComponent>(OwnerActor->GetComponentByClass(UMotionWarpingComponent::StaticClass())))
						{
							WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);
						}
					}
					
					UE_LOG(LogBattleComponent, Warning, TEXT("DoSweep Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);

					SendHitResult(HitActor, Hit, AttackType);
				}
			}
		}
	}
}
