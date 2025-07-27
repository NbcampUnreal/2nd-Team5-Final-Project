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
	FString RowNameString = FString::Printf(TEXT("%s_%s"), 
		*UEnum::GetValueAsString(NewUnitType), 
		*UEnum::GetValueAsString(NewChapterType)
	);
	FName RowName = FName(*RowNameString);

	const FString ContextString(TEXT("AI Unit Stats Lookup"));
	if (FAIUnitStatsData* StatsData = AIStatsDataTable->FindRow<FAIUnitStatsData>(RowName, ContextString, true))
	{
		UE_LOG(LogTemp, Log, TEXT("UAIAttributeComponent: '%s' (유닛: %s, 챕터: %s) 타입의 AI 능력치 데이터 로드 성공."), 
			*RowName.ToString(),
			*UEnum::GetValueAsString(NewUnitType),
			*UEnum::GetValueAsString(NewChapterType)
		);
		ApplyRandomStatsToOwner(*StatsData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UAIAttributeComponent: 데이터 테이블에서 '%s' 타입의 Row를 찾을 수 없습니다! (%s)"), *RowName.ToString(), *GetOwner()->GetName());
	}
}

void UAIAttributeComponent::ApplyRandomStatsToOwner(const FAIUnitStatsData& StatsData)
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor)) return;

	if (ASLMonsterAICharacter* MonsterAI = Cast<ASLMonsterAICharacter>(OwnerActor))
	{
		float RandomHealth = FMath::RandRange(StatsData.HealthMin, StatsData.HealthMax);
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

