#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAILODComponent.generated.h"

class ASLMonsterAICharacter;
class USLAIStateComponent;

UENUM(BlueprintType)
enum class EAILODLevel : uint8
{
	Max    UMETA(DisplayName = "Max"),
	High    UMETA(DisplayName = "High"),
	Medium  UMETA(DisplayName = "Medium"),
	Low     UMETA(DisplayName = "Low"),
	Culled  UMETA(DisplayName = "Culled")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAILODComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAILODComponent();

	EAILODLevel GetCurrentLODLevel() const { return CurrentLODLevel; }

protected:
	virtual void BeginPlay() override;

private:
	EAILODLevel CurrentLODLevel = EAILODLevel::Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings", meta = (AllowPrivateAccess = "true"))
	float LODCheckInterval = 0.25f;
	float LODCheckTimer;

public:
	void SetLODLevel(EAILODLevel NewLevel);
	EAILODLevel CalculateLODLevel(float MaxDetailDistance, float HighDetailDistance, float MediumDetailDistance, float LowDetailDistance) const;
};
