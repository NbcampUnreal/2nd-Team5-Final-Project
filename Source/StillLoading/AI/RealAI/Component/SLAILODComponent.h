#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/BattleManager/SLBattleTypes.h"
#include "Components/ActorComponent.h"
#include "SLAILODComponent.generated.h"

class ASLMonsterAICharacter;
class USLAIStateComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAILODComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAILODComponent();

	EAILODLevel GetCurrentLODLevel() const { return CurrentLODLevel; }
	static FString LODLevelToString(EAILODLevel Level);
	void SetLODLevel(EAILODLevel NewLevel);
	EAILODLevel CalculateLODLevel(float MaxDetailDistance, float HighDetailDistance, float MediumDetailDistance, float LowDetailDistance) const;

protected:
	virtual void BeginPlay() override;

private:
	EAILODLevel CurrentLODLevel = EAILODLevel::Max;
};
