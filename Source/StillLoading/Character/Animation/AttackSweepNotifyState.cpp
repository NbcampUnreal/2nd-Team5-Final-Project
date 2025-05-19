#include "AttackSweepNotifyState.h"

#include "Character/BattleComponent/BattleComponent.h"

void UAttackSweepNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		//UE_LOG(LogTemp, Warning, TEXT("[AttackNotify] Begin - Owner: %s, Duration: %.2f"), *Owner->GetName(), TotalDuration);
	}
}

void UAttackSweepNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UBattleComponent* BattleComp = Owner->FindComponentByClass<UBattleComponent>())
		{
			BattleComp->DoAttackSweep();
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
