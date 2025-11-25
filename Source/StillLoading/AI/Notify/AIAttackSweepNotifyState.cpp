// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAttackSweepNotifyState.h"

#include "AIController.h"
#include "AI/SLAIFunctionLibrary.h"
#include "Character/BattleComponent/BattleComponent.h"

void UAIAttackSweepNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	DoAttackSweep(OwnerActor);
}

void UAIAttackSweepNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	DoAttackSweep(OwnerActor);
}

void UAIAttackSweepNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AlreadyHitActors.Empty();
}

void UAIAttackSweepNotifyState::DoAttackSweep(AActor* OwnerActor)
{
	if (!OwnerActor)
	{
		return;
	}

	UBattleComponent* OwnerBattleComp = OwnerActor->FindComponentByClass<UBattleComponent>();
	ASLAIBaseCharacter* OwnerCharacter= Cast<ASLAIBaseCharacter>(OwnerActor);

	if (!OwnerBattleComp || !OwnerCharacter)
	{
		return;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return;
	}

	FVector Start = OwnerActor->GetActorLocation() + FVector(0, 0, 25);
	FVector End = Start + OwnerActor->GetActorForwardVector() * 100;
	FCollisionShape SweepShape = FCollisionShape::MakeCapsule(45.f, 60.f);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		SweepShape,
		Params
	);


	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}
		
		if (AlreadyHitActors.Contains(HitActor))
		{
			continue;
		}

		if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>(); !IsValid(TargetBattleComp))
		{
			continue;
		}
		
		AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
		bool bIsEnemy = false;
		if (AIController)
		{
			bIsEnemy = USLAIFunctionLibrary::IsEnemy(AIController);
		}

		float DamageMultiplier = 1.f;
		if (bIsEnemy)
		{
			DamageMultiplier = OwnerCharacter->GetChapterDamageMultiplier();
		}
		float Damage = OwnerBattleComp->GetDamageByType(AttackType);
		AlreadyHitActors.Add(HitActor);
		OwnerBattleComp->SendHitResult(HitActor, Hit, AttackType, Damage * DamageMultiplier);
	}
}
