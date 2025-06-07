#include "SLCompanionPatternData.h"

USLCompanionPatternData::USLCompanionPatternData()
{
    MappingDataTable = nullptr;
}

ECompanionActionPattern USLCompanionPatternData::GetActionPatternForTag(FGameplayTag Tag) const
{
    if (const ECompanionActionPattern* FoundPattern = TagToPatternMap.Find(Tag))
    {
        return *FoundPattern;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("No ECompanionActionPattern found for GameplayTag: %s"), *Tag.ToString());
    return ECompanionActionPattern::ECAP_None;
}

FGameplayTag USLCompanionPatternData::GetTagForActionPattern(ECompanionActionPattern Pattern) const
{
    return FindTagForPattern(Pattern);
}

TArray<FGameplayTag> USLCompanionPatternData::GetPatternsByDistanceType(ECompanionDistanceType DistanceType, bool bIsBattleMage) const
{
    const TMap<ECompanionDistanceType, TArray<FGameplayTag>>& PatternMap = bIsBattleMage ? BM_PatternsByDistance : WZ_PatternsByDistance;

    if (const TArray<FGameplayTag>* FoundPatterns = PatternMap.Find(DistanceType))
    {
        return *FoundPatterns;
    }

    return TArray<FGameplayTag>();
}

FGameplayTagContainer USLCompanionPatternData::GetPatternsByDistanceTypeAsContainer(ECompanionDistanceType DistanceType, bool bIsBattleMage) const
{
    TArray<FGameplayTag> PatternArray = GetPatternsByDistanceType(DistanceType, bIsBattleMage);
    
    FGameplayTagContainer PatternContainer;
    for (const FGameplayTag& Tag : PatternArray)
    {
        PatternContainer.AddTag(Tag);
    }
    
    return PatternContainer;
}

FGameplayTagContainer USLCompanionPatternData::FilterPatternsByCharacterType(const FGameplayTagContainer& InputPatterns, bool bIsBattleMage) const
{
    FGameplayTagContainer FilteredPatterns;
    TArray<FGameplayTag> TagArray;
    InputPatterns.GetGameplayTagArray(TagArray);

    for (const FGameplayTag& Tag : TagArray)
    {
        FString TagString = Tag.ToString();
        bool bIsBMPattern = TagString.Contains("BM");
        bool bIsWZPattern = TagString.Contains("WZ");
        
        if ((bIsBattleMage && bIsBMPattern) || (!bIsBattleMage && bIsWZPattern) || (!bIsBMPattern && !bIsWZPattern))
        {
            FilteredPatterns.AddTag(Tag);
        }
    }
    
    return FilteredPatterns;
}

bool USLCompanionPatternData::IsPatternLoop(FGameplayTag Tag) const
{
    if (const bool* FoundLoop = TagToLoopMap.Find(Tag))
    {
        return *FoundLoop;
    }
    
    return false;
}

void USLCompanionPatternData::PostLoad()
{
    Super::PostLoad();
    BuildMappingMap();
}

#if WITH_EDITOR
void USLCompanionPatternData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(USLCompanionPatternData, MappingDataTable))
    {
        BuildMappingMap();
    }
}
#endif

void USLCompanionPatternData::BuildMappingMap()
{
    TagToPatternMap.Empty();
    TagToLoopMap.Empty();
    BM_PatternsByDistance.Empty();
    WZ_PatternsByDistance.Empty();

    if (!MappingDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("MappingDataTable is not set in USLCompanionPatternData: %s"), *GetName());
        return;
    }

    TArray<FSLCompanionActionPatternMappingRow*> Rows;
    MappingDataTable->GetAllRows<FSLCompanionActionPatternMappingRow>(TEXT("USLCompanionPatternData::BuildMappingMap"), Rows);

    Rows.Sort([](const FSLCompanionActionPatternMappingRow& A, const FSLCompanionActionPatternMappingRow& B) {
        return A.Priority < B.Priority;
    });

    for (const FSLCompanionActionPatternMappingRow* Row : Rows)
    {
        if (!Row || !Row->GameplayTag.IsValid())
        {
            continue;
        }

        if (!TagToPatternMap.Contains(Row->GameplayTag))
        {
            TagToPatternMap.Add(Row->GameplayTag, Row->ActionPattern);
            TagToLoopMap.Add(Row->GameplayTag, Row->bIsLoop);
        }

        TMap<ECompanionDistanceType, TArray<FGameplayTag>>& PatternMap = Row->bIsBattleMagePattern ? BM_PatternsByDistance : WZ_PatternsByDistance;

        if (!PatternMap.Contains(Row->DistanceType))
        {
            PatternMap.Add(Row->DistanceType, TArray<FGameplayTag>());
        }

        PatternMap[Row->DistanceType].AddUnique(Row->GameplayTag);
    }
}

FGameplayTag USLCompanionPatternData::FindTagForPattern(ECompanionActionPattern Pattern) const
{
    for (const auto& Pair : TagToPatternMap)
    {
        if (Pair.Value == Pattern)
        {
            return Pair.Key;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No FGameplayTag found for ECompanionActionPattern: %s"), *UEnum::GetValueAsString(Pattern));
    return FGameplayTag();
}