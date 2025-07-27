#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/Data/EAIUnitType.h"
#include "AI/RealAI/Data/FAIUnitStatsData.h"
#include "Components/ActorComponent.h"
#include "AIAttributeComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UAIAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIAttributeComponent();

protected:
	virtual void BeginPlay() override;

	void InitializeData();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	TObjectPtr<UDataTable> AIStatsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	EAIUnitType AIUnitType = EAIUnitType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attributes")
	EAIChapterType AIChapterType = EAIChapterType::None;

	UFUNCTION(BlueprintCallable, Category = "AI Attributes")
	void SetAIStat(EAIChapterType NewChapterType, EAIUnitType NewUnitType);

protected:
	void ApplyRandomStatsToOwner(const FAIUnitStatsData& StatsData);
};
