#include "BattleComponent.h"

#include "GenericTeamAgentInterface.h"
#include "MotionWarpingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DataAsset/HitEffectDataAsset.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogBattleComponent);

UBattleComponent::UBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBattleComponent::SendHitResult(AActor* HitTarget, const FHitResult& HitResult, const EAttackAnimType AnimType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		const IGenericTeamAgentInterface* OwnerGenericTeamID = nullptr;

		// 때린자가 pawn일때 팀 ID 추출
		if (const APawn* OwnerPawn = Cast<APawn>(OwnerActor))
		{
			OwnerGenericTeamID = Cast<IGenericTeamAgentInterface>(OwnerPawn->GetController());
			if (!OwnerGenericTeamID) return;
		}

		// 맞은자가 Pawn일때 TeamID 비교
		if (const APawn* HitTargetPawn = Cast<APawn>(HitTarget))
		{
			if (const IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(
				HitTargetPawn->GetController()))
			{
				if (TeamAgentInterface->GetGenericTeamId() == OwnerGenericTeamID->GetGenericTeamId()) return;
			}
		}

		if (const ASLPlayerCharacter* APlayerCharacter = Cast<ASLPlayerCharacter>(OwnerActor))
		{
			if (ASLBattlePlayerState* PlayerState = APlayerCharacter->GetPlayerState<ASLBattlePlayerState>())
			{
				PlayerState->IncreaseBurningGage(IncreaseBurningGageAmount);
			}
		}

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

void UBattleComponent::ReceiveHitResult(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult,
                                        const EAttackAnimType AnimType)
{
	if (const AActor* OwnerActor = GetOwner())
	{
		UE_LOG(LogBattleComponent, Warning,
		       TEXT("피격 발생! 소유자: %s, 데미지: %.2f, 공격자: %s, 위치: %s, 뼈: %s, AnimType: %s"),
		       *OwnerActor->GetName(),
		       GetDamageByType(AnimType),
		       DamageCauser ? *DamageCauser->GetName() : TEXT("None"),
		       *HitResult.ImpactPoint.ToString(),
		       *HitResult.BoneName.ToString(),
		       *UEnum::GetValueAsString(AnimType)
		);

		OnCharacterHited.Broadcast(DamageCauser, GetDamageByType(AnimType), HitResult, ConvertAttackAnimType(AnimType));

		if (OwnerActor->IsA(AMonsterAICharacter::StaticClass())
			|| OwnerActor->IsA(ASLAIBaseCharacter::StaticClass()))
		{
			if (HitEffectData)
			{
				UNiagaraSystem* EffectToSpawn = HitEffectData->DefaultEffect;

				if (const UCombatHandlerComponent* CombatHandler = DamageCauser->FindComponentByClass<
					UCombatHandlerComponent>())
				{
					if (CombatHandler->IsEmpowered())
					{
						EffectToSpawn = HitEffectData->EmpoweredEffect;
					}
				}

				if (AnimType == EAttackAnimType::AAT_FinalAttackA
					|| AnimType == EAttackAnimType::AAT_FinalAttackB
					|| AnimType == EAttackAnimType::AAT_FinalAttackC)
				{
					EffectToSpawn = HitEffectData->KillMotionEffect;
				}

				if (USkeletalMeshComponent* Mesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
				{
					const FName TargetBone = HitResult.BoneName.IsNone() ? TEXT("root") : HitResult.BoneName;
					UNiagaraFunctionLibrary::SpawnSystemAttached(
						EffectToSpawn,
						Mesh,
						TargetBone,
						FVector::ZeroVector,
						FRotator::ZeroRotator,
						EAttachLocation::SnapToTarget,
						true
					);
				}
			}
		}
		else if (OwnerActor->IsA(ASLPlayerCharacter::StaticClass()))
		{
			UNiagaraSystem* EffectToSpawn = HitEffectData->CharacterHitEffect;

			if (USkeletalMeshComponent* Mesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
			{
				const FName TargetBone = HitResult.BoneName.IsNone() ? TEXT("root") : HitResult.BoneName;
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					EffectToSpawn,
					Mesh,
					TargetBone,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					true
				);
			}
		}
	}
}

void UBattleComponent::DoAttackSweep(EAttackAnimType AttackType)
{
	if (AActor* OwnerActor = GetOwner())
	{
		FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 25);
		FVector End = Start + OwnerActor->GetActorForwardVector() * 80;
		FCollisionShape SweepShape = FCollisionShape::MakeCapsule(45.f, 60.f);

		switch (AttackType)
		{
		case EAttackAnimType::AAT_Airborn:
		case EAttackAnimType::AAT_Skill1:
			SweepShape = FCollisionShape::MakeCapsule(50.f, 70.f);
			break;
		case EAttackAnimType::AAT_Skill2:
			Start = OwnerActor->GetActorLocation() + FVector(0, 0, 0);
			End = Start * 200;
			SweepShape = FCollisionShape::MakeSphere(500.f);
			break;
		default: break;
		}

		if (const UCombatHandlerComponent* CombatHandler = OwnerActor->FindComponentByClass<UCombatHandlerComponent>())
		{
			if (CombatHandler->IsEmpowered())
			{
				SweepShape = FCollisionShape::MakeSphere(100.f);
			}
		}

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
			switch (AttackType)
			{
			case EAttackAnimType::AAT_Skill2:
				DrawDebugSphere(GetWorld(), Start, SweepShape.GetSphereRadius(), 16, FColor::Green, false, 1.0f);
				break;
			default:
				DrawDebugCapsule(GetWorld(), End, SweepShape.GetCapsuleHalfHeight(), SweepShape.GetCapsuleRadius(),
				                 FQuat::Identity, FColor::Green, false, 1.0f);
				break;
			}
		}

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (AttackType == EAttackAnimType::AAT_FinalAttackA
				|| AttackType == EAttackAnimType::AAT_FinalAttackB
				|| AttackType == EAttackAnimType::AAT_FinalAttackC)
			{
				if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
				{
					SendHitResult(HitActor, Hit, AttackType);
					return;
				}
			}

			if (HitActor && HitActor != OwnerActor && !AlreadyHitActors.Contains(HitActor))
			{
				if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
				{
					//UE_LOG(LogBattleComponent, Warning, TEXT("DoAttackSweep Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);

					SendHitResult(HitActor, Hit, AttackType);
				}
			}
		}
	}
}

bool UBattleComponent::DoSweep(EAttackAnimType AttackType)
{
	bool bIsExistAvailTarget = false;

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
			DrawDebugCapsule(GetWorld(), End, SweepShape.GetCapsuleHalfHeight(), SweepShape.GetCapsuleRadius(),
			                 FQuat::Identity, FColor::Green, false, 1.0f);
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
						const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
							TargetLoc, HitActor->GetActorLocation());

						if (UMotionWarpingComponent* WarpComp = Cast<UMotionWarpingComponent>(
							OwnerActor->GetComponentByClass(UMotionWarpingComponent::StaticClass())))
						{
							WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);
						}

						bIsExistAvailTarget = true;
					}

					UE_LOG(LogBattleComponent, Warning, TEXT("DoSweep Hit Actor: %s"), *HitActor->GetName());
					AlreadyHitActors.Add(HitActor);

					SendHitResult(HitActor, Hit, AttackType);
				}
			}
		}
	}

	return bIsExistAvailTarget;
}

EHitAnimType UBattleComponent::ConvertAttackAnimType(EAttackAnimType AttackType)
{
	switch (AttackType)
	{
	case EAttackAnimType::AAT_NormalAttack1:
	case EAttackAnimType::AAT_NormalAttack2:
	case EAttackAnimType::AAT_NormalAttack3:
	case EAttackAnimType::AAT_BM_Attack01: // 약한거
	case EAttackAnimType::AAT_BM_Attack02: // 약한거
	case EAttackAnimType::AAT_BM_Attack03: // 약한거
	case EAttackAnimType::AAT_BM_Attack05: // 약한거
	case EAttackAnimType::AAT_BM_Attack06: // 약한거
	case EAttackAnimType::AAT_BM_Attack09: // 약한거
	case EAttackAnimType::AAT_BM_Attack11: // 약한거
	case EAttackAnimType::AAT_BM_Attack12: // 약한거
	case EAttackAnimType::AAT_WZ_Attack10: // 약한거
	case EAttackAnimType::AAT_WZ_Attack11: // 약한거
	case EAttackAnimType::AAT_WZ_Attack17: // 약한거
	case EAttackAnimType::AAT_WZ_Attack18: // 약한거
	case EAttackAnimType::AAT_WZ_Attack19: // 약한거
	case EAttackAnimType::AAT_DG_Combo_Attack_01: // 약한거
	case EAttackAnimType::AAT_DG_Combo_Attack_02: // 약한거
	case EAttackAnimType::AAT_DG_Combo_Attack_03: // 약한거
	case EAttackAnimType::AAT_DG_Combo_Attack_04: // 약한거
	case EAttackAnimType::AAT_DG_Combo_Attack_Air: // 약한거
	case EAttackAnimType::AAT_DG_Attack_Air_To_Floor: // 약한거
		return EHitAnimType::HAT_WeakHit;
		
	case EAttackAnimType::AAT_SpecialAttack1:
	case EAttackAnimType::AAT_SpecialAttack2:
	case EAttackAnimType::AAT_SpecialAttack3:
	case EAttackAnimType::AAT_FootAttack_Left: // 중간거
	case EAttackAnimType::AAT_FootAttack_Right: // 중간거
	case EAttackAnimType::AAT_GroundSlam_02: // 중간거
	case EAttackAnimType::AAT_GroundSlam_01: // 중간거
	case EAttackAnimType::AAT_JumpAttack: // 중간거
	case EAttackAnimType::AAT_Whirlwind: // 중간거
	case EAttackAnimType::AAT_BM_Attack04: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack07: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack08: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack10: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack13: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack14: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack15: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack16: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack17: // 중간거 (한발짝)
	case EAttackAnimType::AAT_BM_Attack18: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack01: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack02: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack03: // 기절
	case EAttackAnimType::AAT_WZ_Attack04: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack06: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack07: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack12: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack13: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack14: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack15: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack16: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack20: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack21: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack22: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Attack23: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Special_Patterns1: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Special_Patterns2: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Special_Patterns3: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Special_Patterns4: // 중간거 (한발짝)
	case EAttackAnimType::AAT_WZ_Special_Patterns5: // 중간거 (한발짝)
	case EAttackAnimType::AAT_DG_Run_Attack_02:  // 중간거 (한발짝)
	case EAttackAnimType::AAT_DG_Dash_Air_Attack: // 중간거 (한발짝)
		return EHitAnimType::HAT_HardHit;
		
	case EAttackAnimType::AAT_AirAttack1:
	case EAttackAnimType::AAT_AirAttack2:
	case EAttackAnimType::AAT_AirAttack3:
		return EHitAnimType::HAT_AirHit;
		
	case EAttackAnimType::AAT_Airborn:
		return EHitAnimType::HAT_AirBorne;
		
	case EAttackAnimType::AAT_Skill1:
	case EAttackAnimType::AAT_DG_Attack_Up_01: // 날라가는거 위로
	case EAttackAnimType::AAT_DG_Attack_Up_Floor_To_Air_02: // 날라가는거 위로
		return EHitAnimType::HAT_AirUp;
		
	case EAttackAnimType::AAT_Skill2:
	case EAttackAnimType::AAT_Attack_01: // 날라가는거
	case EAttackAnimType::AAT_Attack_02: // 날라가는거
	case EAttackAnimType::AAT_Attack_04: // 날라가는거
	case EAttackAnimType::AAT_DashAttack: // 날라가는거
	case EAttackAnimType::AAT_ThrowStone: // 날라가는거
	case EAttackAnimType::AAT_WZ_Attack05: // 날라가는거
	case EAttackAnimType::AAT_WZ_Attack09: // 날라가는거
		return EHitAnimType::HAT_FallBack;

	case EAttackAnimType::AAT_Attack_03: // 기절
	case EAttackAnimType::AAT_WZ_Attack08: //기절
	case EAttackAnimType::AAT_WZ_Loop_Attack04: // 기절
	case EAttackAnimType::AAT_DG_Parry: // 기절
		return EHitAnimType::HAT_Exhausterd;
		
	case EAttackAnimType::AAT_AINormal:
		return EHitAnimType::HAT_WeakHit;
		
	case EAttackAnimType::AAT_AISpecial:
		return EHitAnimType::HAT_HardHit;
		
	case EAttackAnimType::AAT_FinalAttackA:
		return EHitAnimType::HAT_KillMotionA;
	case EAttackAnimType::AAT_FinalAttackB:
		return EHitAnimType::HAT_KillMotionB;
	case EAttackAnimType::AAT_FinalAttackC:
		return EHitAnimType::HAT_KillMotionC;
		
	case EAttackAnimType::AAT_ParryAttack:
		return EHitAnimType::HAT_Parry;
	default: break;
	}

	return EHitAnimType::HAT_None;
}

void UBattleComponent::ClearHitTargets()
{
	AlreadyHitActors.Empty();
}

float UBattleComponent::GetDamageByType(EAttackAnimType InType) const
{
	if (!AttackData) return 0;

	for (const auto& [AttackType, DamageAmount] : AttackData->AttackDataList)
	{
		if (AttackType == InType)
		{
			return DamageAmount;
		}
	}
	return 0.0f;
}

void UBattleComponent::OnEnemyDeath_Implementation(AActor* DeadAI)
{
	if (const ASLPlayerCharacter* OwnerChar = Cast<ASLPlayerCharacter>(GetOwner()))
	{
		if (UMovementHandlerComponent* MovementHandler = OwnerChar->FindComponentByClass<UMovementHandlerComponent>())
		{
			if (MovementHandler->CameraFocusTarget == DeadAI)
			{
				MovementHandler->DisableLock();
			}
		}
	}
}
