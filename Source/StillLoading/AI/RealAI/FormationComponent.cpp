#include "FormationComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFormationComponent::StartFidgetingFor(const TArray<TObjectPtr<AActor>>& AgentsToFidget)
{
	FidgetStates.Empty();

	for (const auto& Agent : AgentsToFidget)
	{
		if (Agent)
		{
			FFidgetState NewState;
			NewState.HomeLocation = Agent->GetActorLocation();
			NewState.FidgetTimer = FMath::RandRange(0.f, static_cast<float>(FidgetInterval.X));
            
			FidgetStates.Add(Agent, NewState);
		}
	}
}

void UFormationComponent::StopFidgeting()
{
	FidgetStates.Empty();
}

void UFormationComponent::UpdateFidgeting(float DeltaTime)
{
	for (auto& Elem : FidgetStates)
	{
		AActor* Agent = Elem.Key.Get();
		FFidgetState& State = Elem.Value;

		if (!Agent) continue;

		State.FidgetTimer -= DeltaTime;

		if (State.FidgetTimer <= 0.f)
		{
			AController* Con = Agent->GetInstigatorController();
			AAIController* AICon = Cast<AAIController>(Con);

			if (AICon && AICon->GetMoveStatus() == EPathFollowingStatus::Type::Idle)
			{
				const FVector RightVector = Agent->GetActorRightVector();
				const float RandomOffset = FMath::RandRange(-FidgetRange, FidgetRange);
				const FVector NewLocation = State.HomeLocation + RightVector * RandomOffset;

				AICon->MoveToLocation(NewLocation, 5.f);
			}
            
			State.FidgetTimer = FMath::RandRange(FidgetInterval.X, FidgetInterval.Y);
		}
	}
}

void UFormationComponent::AssignStoredFormation(
	AActor* InLeader, 
	const TArray<TObjectPtr<AActor>>& InFollowers,
	const TArray<TObjectPtr<AActor>>& InAttackingAgents,
	EFormationType FormationType, 
	float Spacing)
{
	if (InFollowers.IsEmpty() || !InLeader) return;
    
	TArray<AActor*> NonAttackingFollowers;
	for (AActor* Agent : InFollowers)
	{
		if (Agent && !InAttackingAgents.Contains(Agent))
		{
			NonAttackingFollowers.Add(Agent);
		}
	}
    
	if (NonAttackingFollowers.IsEmpty()) return;

	const FRotator LeaderRotation = InLeader->GetActorRotation();
	const FVector LeaderForward = InLeader->GetActorForwardVector();
	
	const float LeaderOffsetDistance = -200.f;
	const FVector FormationCenter = InLeader->GetActorLocation() - LeaderForward * LeaderOffsetDistance;
	const FRotator FormationRotation = FRotator(0.f, LeaderRotation.Yaw + 180.f, 0.f);

	TArray<FVector> SlotPositions = CalculateFormationSlots(FormationType, FormationCenter, NonAttackingFollowers.Num(), Spacing, FormationRotation);

	for (int32 i = 0; i < NonAttackingFollowers.Num(); ++i)
	{
		APawn* Pawn = Cast<APawn>(NonAttackingFollowers[i]);
		if (!Pawn) continue;
       
		const float DistanceToSlot = FVector::Dist(Pawn->GetActorLocation(), SlotPositions[i]);

		if (DistanceToSlot > FormationAcceptanceRadius)
		{
			AAIController* AICon = Cast<AAIController>(Pawn->GetController());
			if (AICon)
			{
				AICon->MoveToLocation(SlotPositions[i], 50.f);
			}
		}
		else
		{
			const FRotator CurrentRotation = Pawn->GetActorRotation();
			const FRotator TargetRotation = LeaderRotation;
          
			const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
          
			Pawn->SetActorRotation(SmoothedRotation);
		}
	}
}

TArray<FVector> UFormationComponent::CalculateFormationSlots(EFormationType Type, const FVector& Center, int32 NumAgents, float Spacing, const FRotator& Rotation) const
{
	TArray<FVector> Slots;

	switch (Type)
	{
	case EFormationType::Wedge:
		{
			int32 Half = NumAgents / 2;
			for (int32 i = -Half; i <= Half; ++i)
			{
				if (i == 0 && NumAgents % 2 == 0) continue;
				FVector Offset = FVector(-FMath::Abs(i) * Spacing, i * Spacing, 0.f);
				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
			}
			break;
		}
	case EFormationType::Line:
		{
			for (int32 Index = 0; Index < NumAgents; ++Index)
			{
				const int32 MaxPerRow = 5;
				int32 Row = Index / MaxPerRow;
				int32 Col = Index % MaxPerRow;
				int32 Half = MaxPerRow / 2;

				int32 YOffsetIndex = Col - Half;
				if (MaxPerRow % 2 == 0 && Col >= Half) YOffsetIndex += 1;

				FVector Offset = FVector(-Row * Spacing, YOffsetIndex * Spacing, 0.f);
				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
			}
			break;
		}
	case EFormationType::Squad:
		{
			const int32 SquadSize = 4;
			const int32 NumSquads = FMath::CeilToInt((float)NumAgents / SquadSize);

			for (int32 SquadIndex = 0; SquadIndex < NumSquads; ++SquadIndex)
			{
				for (int32 i = 0; i < SquadSize; ++i)
				{
					int32 AgentIndex = SquadIndex * SquadSize + i;
					if (AgentIndex >= NumAgents) break;

					int32 Row = i / 2;
					int32 Col = i % 2;

					float SquadYOffset = (SquadIndex - (NumSquads - 1) / 2.0f) * Spacing * 3.0f;

					FVector LocalOffset = FVector(-Row * Spacing, (Col - 0.5f) * 2 * Spacing + SquadYOffset, 0.f);

					FVector RotatedOffset = Rotation.RotateVector(LocalOffset);
					Slots.Add(Center + RotatedOffset);
				}
			}
			break;
		}
	case EFormationType::Circle:
		{
			if (NumAgents <= 0) break;

			float CustomSpace = 0.f;
			if (Spacing < 200.f)
			{
				CustomSpace = 200.0f;
			}
			CustomSpace = Spacing;
			const float Radius = CustomSpace;
          
			if (NumAgents == 1)
			{
				FVector Offset = FVector(Radius, 0.f, 0.f);
				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
				break;
			}

			const float AngleStep = FMath::DegreesToRadians(180.f) / (NumAgents - 1);
			const float StartAngle = -FMath::DegreesToRadians(90.f);

			for (int32 i = 0; i < NumAgents; ++i)
			{
				const float CurrentAngle = StartAngle + i * AngleStep;

				FVector Offset = FVector(Radius * FMath::Cos(CurrentAngle), Radius * FMath::Sin(CurrentAngle), 0.f);

				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
			}
			break;
		}
	case EFormationType::BackCircle:
		{
			if (NumAgents <= 0) break;

			const float Radius = Spacing;
			const float BackwardOffset = 100.f;

			if (NumAgents == 1)
			{
				FVector Offset = FVector(Radius + BackwardOffset, 0.f, 0.f);
				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
				break;
			}

			const float AngleStep = FMath::DegreesToRadians(180.f) / (NumAgents - 1);
			const float StartAngle = -FMath::DegreesToRadians(90.f);

			for (int32 i = 0; i < NumAgents; ++i)
			{
				const float CurrentAngle = StartAngle + i * AngleStep;

				FVector Offset = FVector(
				Radius * FMath::Cos(CurrentAngle) + BackwardOffset,
				Radius * FMath::Sin(CurrentAngle), 
				0.f
			 );

				FVector RotatedOffset = Rotation.RotateVector(Offset);
				Slots.Add(Center + RotatedOffset);
			}
			break;
		}
	}

	return Slots;
}

