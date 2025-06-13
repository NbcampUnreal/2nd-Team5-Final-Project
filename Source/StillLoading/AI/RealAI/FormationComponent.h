#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationComponent.generated.h"

class AMonsterAICharacter;

UENUM(BlueprintType)
enum class EFormationType : uint8
{
	Wedge	UMETA(DisplayName = "Wedge"),
	Line	UMETA(DisplayName = "Line"),
	Squad	UMETA(DisplayName = "Squad"),
	Circle  UMETA(DisplayName = "Circle"),
	BackCircle  UMETA(DisplayName = "BackCircle"),
};

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	Attack	UMETA(DisplayName = "Attack"),
	HoldPosition	UMETA(DisplayName = "HoldPosition"),
};

USTRUCT()
struct FFidgetState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector HomeLocation = FVector::ZeroVector;

	UPROPERTY()
	float FidgetTimer = 0.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationComponent();

	void AssignStoredFormation(
	   AActor* InLeader, 
	   const TArray<TObjectPtr<AActor>>& InFollowers, 
	   const TArray<TObjectPtr<AActor>>& InAttackingAgents,
	   EFormationType FormationType, 
	   float Spacing
   );

	void StartFidgetingFor(const TArray<TObjectPtr<AActor>>& AgentsToFidget);
	void StopFidgeting();
	void UpdateFidgeting(float DeltaTime);

protected:
	virtual void BeginPlay() override;
	
private:
	TArray<FVector> CalculateFormationSlots(EFormationType Type, const FVector& Center, int32 NumAgents, float Spacing, const FRotator& Rotation) const;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, FFidgetState> FidgetStates;
    
	UPROPERTY(EditAnywhere, Category = "AI|Fidget")
	float FidgetRange = 50.f;

	UPROPERTY(EditAnywhere, Category = "AI|Fidget")
	float FormationAcceptanceRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "AI|Fidget")
	FVector2D FidgetInterval = FVector2D(2.0f, 5.0f);
};
