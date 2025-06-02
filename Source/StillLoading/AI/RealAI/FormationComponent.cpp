#include "FormationComponent.h"
#include "AIController.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFormationComponent::SetAgentList(const TArray<AActor*>& Agents)
{
	StoredAgents = Agents;
}

void UFormationComponent::AssignStoredFormation(EFormationType FormationType, float Spacing)
{
	if (StoredAgents.Num() <= 1) return;

	AActor* Leader = StoredAgents[0];
	FRotator LeaderRotation = Leader->GetActorRotation();
	FVector LeaderForward = Leader->GetActorForwardVector();

	float LeaderOffsetDistance = -200.f;
	FVector FormationCenter = Leader->GetActorLocation() - LeaderForward * LeaderOffsetDistance;

	FRotator FormationRotation = FRotator(0.f, LeaderRotation.Yaw + 180.f, 0.f);

	TArray<AActor*> Followers;
	for (int32 i = 1; i < StoredAgents.Num(); ++i)
	{
		if (StoredAgents[i]) Followers.Add(StoredAgents[i]);
	}

	TArray<FVector> SlotPositions = CalculateFormationSlots(FormationType, FormationCenter, Followers.Num(), Spacing, FormationRotation);

	for (int32 i = 0; i < Followers.Num(); ++i)
	{
		APawn* Pawn = Cast<APawn>(Followers[i]);
		AAIController* AICon = Cast<AAIController>(Pawn ? Pawn->GetController() : nullptr);

		if (AICon && Pawn)
		{
			AICon->MoveToLocation(SlotPositions[i], 50.f);
			Pawn->SetActorRotation(LeaderRotation);
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
	}

	return Slots;
}

