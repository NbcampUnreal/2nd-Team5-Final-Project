#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/Data/EAIUnitType.h"
#include "AI/RealAI/Data/FAIUnitStatsData.h"
#include "Components/ActorComponent.h"
#include "SLAIAttributeComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAIAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAIAttributeComponent();

protected:
	virtual void BeginPlay() override;

	void InitializeData();

public:
	UFUNCTION(BlueprintCallable, Category = "AI Attributes")
	void SetAIStat(EAIChapterType NewChapterType, EAIUnitType NewUnitType);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	TObjectPtr<UDataTable> AIStatsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	EAIUnitType AIUnitType = EAIUnitType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	EAIChapterType AIChapterType = EAIChapterType::None;

	UFUNCTION(BlueprintCallable)
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintCallable)
	float GetAbleDistance() const { return AvailDistance; }

protected:
	void ApplyRandomStatsToOwner(const FAIUnitStatsData& StatsData);

	float AttackRange = 150.f;
	float AvailDistance = 150.f;
};
