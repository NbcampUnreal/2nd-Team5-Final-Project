#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationComponent.generated.h"

UENUM(BlueprintType)
enum class EFormationType : uint8
{
	Wedge	UMETA(DisplayName = "Wedge"),
	Line	UMETA(DisplayName = "Line"),
	Squad	UMETA(DisplayName = "Squad"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationComponent();
	
	UFUNCTION(BlueprintCallable)
	void AssignStoredFormation(EFormationType FormationType, float Spacing);
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	TArray<FVector> CalculateFormationSlots(EFormationType Type, const FVector& Center, int32 NumAgents, float Spacing, const FRotator& Rotation) const;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> StoredAgents;

public:
	UFUNCTION(BlueprintCallable)
	void SetAgentList(const TArray<AActor*>& Agents);
};
