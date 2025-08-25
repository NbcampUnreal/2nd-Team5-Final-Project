#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAILODComponent.generated.h"

class ASLMonsterAICharacter;
class USLAIStateComponent;

UENUM(BlueprintType)
enum class EAILODLevel : uint8
{
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

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	EAILODLevel CurrentLODLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings", meta = (AllowPrivateAccess = "true"))
	float HighDetailDistance = 2000.f; // 25m

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings", meta = (AllowPrivateAccess = "true"))
	float MediumDetailDistance = 3000.f; // 60m

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings", meta = (AllowPrivateAccess = "true"))
	float LowDetailDistance = 6000.f; // 120m

	// LOD 상태를 얼마나 자주 체크할 것인지 (매 프레임 체크하는 것을 방지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI LOD Settings", meta = (AllowPrivateAccess = "true"))
	float LODCheckInterval = 0.25f;

	float LODCheckTimer;

	UPROPERTY()
	TObjectPtr<ASLMonsterAICharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<USLAIStateComponent> StateComponent;

	void SetLODLevel(EAILODLevel NewLevel);
	EAILODLevel CalculateLODLevel() const;
};
