#include "AIAttributeComponent.h"

#include "AI/RealAI/SLMonsterAICharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UAIAttributeComponent::UAIAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeData();
}

void UAIAttributeComponent::InitializeData()
{
	if (!AIStatsDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UAIAttributeComponent: AIStatsDataTable이 설정되지 않았습니다! (%s)"), *GetOwner()->GetName());
		return;
	}

	if (AIUnitType == EAIUnitType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAIAttributeComponent: AIUnitType이 'None'으로 설정되었습니다. (%s)"), *GetOwner()->GetName());
	}
	
	EAIChapterType CurrentChapter = AIChapterType;
	if (CurrentChapter == EAIChapterType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAIAttributeComponent: AIChapterType이 'None'이거나 유효한 챕터 정보를 가져오지 못했습니다. (%s)"), *GetOwner()->GetName());
	}

	SetAIStat(AIChapterType, AIUnitType);
}

void UAIAttributeComponent::SetAIStat(EAIChapterType NewChapterType, EAIUnitType NewUnitType)
{
	const UEnum* UnitTypeEnum = StaticEnum<EAIUnitType>();
    const UEnum* ChapterTypeEnum = StaticEnum<EAIChapterType>();

    if (!UnitTypeEnum || !ChapterTypeEnum)
    {
        UE_LOG(LogTemp, Error, TEXT("UAIAttributeComponent: 유닛 또는 챕터 Enum 데이터를 찾을 수 없습니다!"));
        return;
    }

	if (NewUnitType == EAIUnitType::Ranger)
	{
		AttackRange = 600.f;
	}

    FString UnitTypeString = UnitTypeEnum->GetNameByValue(static_cast<int64>(NewUnitType)).ToString();
    FString ChapterTypeString = ChapterTypeEnum->GetNameByValue(static_cast<int64>(NewChapterType)).ToString();

    UnitTypeString.RemoveFromStart(TEXT("EAIUnitType::"));
    ChapterTypeString.RemoveFromStart(TEXT("EAIChapterType::"));

    FString RowNameString = FString::Printf(TEXT("%s%s"),
       *UnitTypeString,
       *ChapterTypeString
    );
    FName RowName = FName(*RowNameString);

    const FString ContextString(TEXT("AI Unit Stats Lookup"));
    if (FAIUnitStatsData* StatsData = AIStatsDataTable->FindRow<FAIUnitStatsData>(RowName, ContextString, true))
    {
       ApplyRandomStatsToOwner(*StatsData);
    }
    else
    {
       UE_LOG(LogTemp, Error, TEXT("UAIAttributeComponent: 데이터 테이블에서 타입의 Row를 찾을 수 없습니다!"));
    }
}

void UAIAttributeComponent::ApplyRandomStatsToOwner(const FAIUnitStatsData& StatsData)
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor)) return;

	if (ASLMonsterAICharacter* MonsterAI = Cast<ASLMonsterAICharacter>(OwnerActor))
	{
		float RandomHealth = FMath::RandRange(StatsData.HealthMin, StatsData.HealthMax);
		MonsterAI->MaxHealth = RandomHealth;
		MonsterAI->CurrentHealth = RandomHealth;
		UE_LOG(LogTemp, Log, TEXT("  - 체력 (랜덤): %.2f (%.2f ~ %.2f)"), RandomHealth, StatsData.HealthMin, StatsData.HealthMax);
	}

	if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor))
	{
		if (UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement())
		{
			float RandomSpeed = FMath::RandRange(StatsData.MovementSpeedMin, StatsData.MovementSpeedMax);
			MovementComp->MaxWalkSpeed = RandomSpeed;
			UE_LOG(LogTemp, Log, TEXT("  - 이동 속도 (랜덤): %.2f (%.2f ~ %.2f)"), RandomSpeed, StatsData.MovementSpeedMin, StatsData.MovementSpeedMax);
		}
	}
}

