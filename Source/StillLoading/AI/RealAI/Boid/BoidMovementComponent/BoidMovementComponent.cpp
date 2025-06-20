#include "BoidMovementComponent.h"

#include "AI/RealAI/Boid/SwarmManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBoidMovementComponent::UBoidMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBoidMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	SwarmManager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
}

void UBoidMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerCharacter || !SwarmManager)
	{
		return;
	}

	// 1. 주변 개체 감지 (Sphere Overlap 사용)
	TArray<AActor*> Neighbors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		OwnerCharacter->GetActorLocation(),
		this->PerceptionRadius, // 컴포넌트 자신의 PerceptionRadius 값을 사용
		ObjectTypes,
		AActor::StaticClass(),
		TArray<AActor*>({OwnerCharacter}),
		Neighbors
	);

	// 2. 각 규칙에 따른 힘 계산
	FVector SeparationForce = CalculateSeparationForce(Neighbors) * SwarmManager->SeparationWeight;
	FVector AlignmentForce = CalculateAlignmentForce(Neighbors) * SwarmManager->AlignmentWeight;
	FVector CohesionForce = CalculateCohesionForce(Neighbors) * SwarmManager->CohesionWeight;
	FVector GoalSeekingForce = CalculateGoalSeekingForce() * SwarmManager->GoalSeekingWeight;

	// 3. 모든 힘을 합산
	FVector TotalForce = SeparationForce + AlignmentForce + CohesionForce + GoalSeekingForce;
    
	// 4. 합산된 힘을 캐릭터의 이동 입력으로 전달
	if (!TotalForce.IsNearlyZero())
	{
		OwnerCharacter->AddMovementInput(TotalForce.GetSafeNormal());
        
		// 캐릭터가 이동 방향을 바라보도록 회전
		FRotator TargetRotation = TotalForce.Rotation();
		OwnerCharacter->GetController()->SetControlRotation(TargetRotation);
	}
}

FVector UBoidMovementComponent::CalculateSeparationForce(const TArray<AActor*>& Neighbors)
{
	FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;

	for (AActor* Neighbor : Neighbors)
	{
		FVector ToNeighbor = OwnerCharacter->GetActorLocation() - Neighbor->GetActorLocation();
		if (ToNeighbor.Size() > 0)
		{
			Force += ToNeighbor.GetSafeNormal() / ToNeighbor.Size();
		}
	}
	return Force / Neighbors.Num();
}

FVector UBoidMovementComponent::CalculateAlignmentForce(const TArray<AActor*>& Neighbors)
{
	FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;
    
	for (AActor* Neighbor : Neighbors)
	{
		Force += Neighbor->GetVelocity();
	}
	return (Force / Neighbors.Num()).GetSafeNormal();
}

FVector UBoidMovementComponent::CalculateCohesionForce(const TArray<AActor*>& Neighbors)
{
	FVector Force = FVector::ZeroVector;
	if (Neighbors.Num() == 0) return Force;

	FVector CenterOfMass = FVector::ZeroVector;
	for (AActor* Neighbor : Neighbors)
	{
		CenterOfMass += Neighbor->GetActorLocation();
	}
	CenterOfMass /= Neighbors.Num();

	return (CenterOfMass - OwnerCharacter->GetActorLocation()).GetSafeNormal();
}

FVector UBoidMovementComponent::CalculateGoalSeekingForce()
{
	if (SwarmManager && SwarmManager->bHasGoal)
	{
		return (SwarmManager->SwarmGoalLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();
	}
	return FVector::ZeroVector;
}
