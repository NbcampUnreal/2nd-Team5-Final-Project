#include "AttackSweepNotifyState.h"

#include "Character/BattleComponent/BattleComponent.h"

void UAttackSweepNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AttackType == EAttackAnimType::AAT_FinalAttackA
		|| AttackType == EAttackAnimType::AAT_FinalAttackB
		|| AttackType == EAttackAnimType::AAT_FinalAttackC)
	{
		if (AActor* Owner = MeshComp->GetOwner())
		{
			if (UBattleComponent* BattleComp = Owner->FindComponentByClass<UBattleComponent>())
			{
				BattleComp->DoAttackSweep(AttackType);
			}
		}
	}
}

void UAttackSweepNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AttackType == EAttackAnimType::AAT_FinalAttackA
		|| AttackType == EAttackAnimType::AAT_FinalAttackB
		|| AttackType == EAttackAnimType::AAT_FinalAttackC)
	{
		return;
	}

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UBattleComponent* BattleComp = Owner->FindComponentByClass<UBattleComponent>())
		{
			BattleComp->DoAttackSweep(AttackType);
		}
	}
}

void UAttackSweepNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UBattleComponent* BattleComp = Owner->FindComponentByClass<UBattleComponent>())
		{
			BattleComp->ClearHitTargets();
		}
	}
}
