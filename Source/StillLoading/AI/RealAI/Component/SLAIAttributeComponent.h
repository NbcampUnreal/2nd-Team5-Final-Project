#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/Data/EAIUnitType.h"
#include "AI/RealAI/Data/FAIUnitStatsData.h"
#include "Components/ActorComponent.h"
#include "SaveLoad/SLSaveDataStructs.h"
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

	UFUNCTION(BlueprintCallable, Category = "AI Attributes")
	void ToggleBerserkMode(bool bEnable);

	bool IsBerserkModeActive() const { return bIsBerserkModeActive; }

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
	float AnimRateScale = 1.0f;
	float OriginalMaxWalkSpeed = 0.f;

private:
	/**
	* EAIChapterType을 ESLChapterType으로 변환합니다.
	* @param AIType 변환할 EAIChapterType 값
	* @return 매핑되는 ESLChapterType 값
	*/
	ESLChapterType ConvertToESLChapterType(EAIChapterType AIType);

	/**
	* ESLChapterType을 EAIChapterType으로 변환합니다.
	* @param SLType 변환할 ESLChapterType 값
	* @return 매핑되는 EAIChapterType 값
	*/
	EAIChapterType ConvertToEAIChapterType(ESLChapterType SLType);

	bool bIsBerserkModeActive = false;
};
