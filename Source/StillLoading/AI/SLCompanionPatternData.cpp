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

/**
 * 매핑 데이터 테이블 기반으로 태그와 패턴 간 매핑 및 거리 유형에 따른 패턴 분포를 초기화하고 구성합니다.
 *
 * 이 함수는 내부적으로 TagToPatternMap, BM_PatternsByDistance, WZ_PatternsByDistance 맵을 비우고
 * 새로운 데이터를 채워 넣습니다. 매핑 데이터 테이블이 설정되지 않은 경우 로그 경고를 출력하고 종료됩니다.
 *
 * 매핑 데이터 테이블에서 데이터를 정렬(Priority 값 기준)하고 유효한 데이터를 선택하여
 * 각 태그와 패턴 간의 연관성을 TagToPatternMap에 저장하며,
 * 거리 유형 및 캐릭터 타입(전투 마법사 여부)에 따라 BM_PatternsByDistance 또는 WZ_PatternsByDistance에 분류합니다.
 *
 * 비고:
 * - TagToPatternMap: 게임플레이 태그와 동작 패턴 간 매핑 정보를 담은 맵.
 * - BM_PatternsByDistance: 전투 마법사를 위한 거리 유형별 게임플레이 태그 분류.
 * - WZ_PatternsByDistance: 일반 캐릭터를 위한 거리 유형별 게임플레이 태그 분류.
 */
void USLCompanionPatternData::BuildMappingMap()
{
    TagToPatternMap.Empty();
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
        }

        TMap<ECompanionDistanceType, TArray<FGameplayTag>>& PatternMap = Row->bIsBattleMagePattern ? BM_PatternsByDistance : WZ_PatternsByDistance;

        if (!PatternMap.Contains(Row->DistanceType))
        {
            PatternMap.Add(Row->DistanceType, TArray<FGameplayTag>());
        }

        PatternMap[Row->DistanceType].AddUnique(Row->GameplayTag);
    }

    /*UE_LOG(LogTemp, Log, TEXT("USLCompanionPatternData built %d tag mappings and distance classifications."), TagToPatternMap.Num());*/
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

TArray<FGameplayTag> USLCompanionPatternData::GetPatternsByDistanceType(ECompanionDistanceType DistanceType, bool bIsBattleMage) const
{
    const TMap<ECompanionDistanceType, TArray<FGameplayTag>>& PatternMap = bIsBattleMage ? BM_PatternsByDistance : WZ_PatternsByDistance;

    if (const TArray<FGameplayTag>* FoundPatterns = PatternMap.Find(DistanceType))
    {
        return *FoundPatterns;
    }

    return TArray<FGameplayTag>();
}

/**
 * 주어진 거리 유형 및 전투 마법사 여부에 따라 GameplayTag 패턴 배열로부터
 * FGameplayTagContainer 객체를 생성하여 반환합니다.
 *
 * @param DistanceType 거리 유형. ECompanionDistanceType 열거형 값을 지정.
 * @param bIsBattleMage 전투 마법사 여부. true일 경우 전투 마법사 관련 패턴을, false일 경우 일반 패턴을 반환.
 * @return 해당 거리 유형에 맞는 FGameplayTagContainer 객체.
 */
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

/**
 * 주어진 GameplayTagContainer의 태그들을 캐릭터 유형(전투 마법사 여부)에 따라 필터링하여,
 * 해당 조건에 맞는 태그들만 포함된 새 GameplayTagContainer를 반환합니다.
 *
 * @param InputPatterns 필터링 대상이 되는 FGameplayTagContainer 객체.
 * @param bIsBattleMage 전투 마법사 여부. true일 경우 전투 마법사 관련 태그를 필터링하고, false일 경우 일반 태그를 필터링.
 * @return 캐릭터 유형에 따라 필터링된 태그들만 포함된 FGameplayTagContainer 객체.
 */
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