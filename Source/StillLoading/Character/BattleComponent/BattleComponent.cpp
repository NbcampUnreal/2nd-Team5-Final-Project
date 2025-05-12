#include "BattleComponent.h"

#include "Components/CapsuleComponent.h"

UBattleComponent::UBattleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBattleComponent::PerformAttack()
{
	bool bIsMultiplayer = bForceMultiplayerMode || (GetOwner()->GetNetMode() != NM_Standalone);

	if (bIsMultiplayer && !GetOwner()->HasAuthority())
	{
		ServerPerformAttack();
		return;
	}

	DoAttack();
}

void UBattleComponent::ServerPerformAttack_Implementation()
{
	DoAttack();
}

bool UBattleComponent::ServerPerformAttack_Validate()
{
	return true;
}

void UBattleComponent::DoAttack()
{
	const AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector Start = Owner->GetActorLocation() + FVector(0, 0, 50);
	const FVector End = Start + Owner->GetActorForwardVector() * AttackRange;

	TArray<FHitResult> HitResults;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		EnemyChannel,
		Sphere
	);

	if (bShowDebugLine)
	{
		DrawDebugSphere(GetWorld(), Start, AttackRadius, 12, FColor::Blue, false, 1.f);
		DrawDebugSphere(GetWorld(), End, AttackRadius, 12, FColor::Red, false, 1.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 2.f);
	}
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			
			if (HitActor && HitActor != Owner &&
				HitActor->GetClass()->ImplementsInterface(USLBattleInterface::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor[%s]"), *HitActor->GetName());
			}
			
			/*
			if (HitActor && HitActor != Owner &&
				HitActor->GetClass()->ImplementsInterface(USLBattleInterface::StaticClass()))
			{
				ISLBattleInterface::Execute_ReceiveBattleDamage(HitActor, 10.0f);
			}
			*/
		}
	}
}