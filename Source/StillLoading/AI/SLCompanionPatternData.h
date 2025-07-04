// SLCompanionPatternData.h
#pragma once

#include "CoreMinimal.h"
#include "Companion/SLCompanionCharacter.h"
#include "Engine/DataAsset.h"
#include "SLCompanionPatternData.generated.h"

UCLASS()
class STILLLOADING_API USLCompanionPatternData : public UDataAsset
{
	GENERATED_BODY()

public:
	USLCompanionPatternData();

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Mapping")
	ECompanionActionPattern GetActionPatternForTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Mapping") 
	FGameplayTag GetTagForActionPattern(ECompanionActionPattern Pattern) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Distance")
	TArray<FGameplayTag> GetPatternsByDistanceType(ECompanionDistanceType DistanceType, bool bIsBattleMage) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Distance")
	FGameplayTagContainer GetPatternsByDistanceTypeAsContainer(ECompanionDistanceType DistanceType, bool bIsBattleMage) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Filter")
	FGameplayTagContainer FilterPatternsByCharacterType(const FGameplayTagContainer& InputPatterns, bool bIsBattleMage) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags|Loop")
	bool IsPatternLoop(FGameplayTag Tag) const;

protected:
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void BuildMappingMap();

private:
	FGameplayTag FindTagForPattern(ECompanionActionPattern Pattern) const;

	UPROPERTY(EditDefaultsOnly, Category = "Mapping")
	TObjectPtr<UDataTable> MappingDataTable;

	UPROPERTY(Transient)
	TMap<FGameplayTag, ECompanionActionPattern> TagToPatternMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, bool> TagToLoopMap;

	TMap<ECompanionDistanceType, TArray<FGameplayTag>> BM_PatternsByDistance;
	TMap<ECompanionDistanceType, TArray<FGameplayTag>> WZ_PatternsByDistance;
};