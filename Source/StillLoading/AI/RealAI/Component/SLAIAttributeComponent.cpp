#include "SLAIAttributeComponent.h"

#include "AI/RealAI/SLMonsterAICharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SubSystem/SLLevelTransferSubsystem.h"

USLAIAttributeComponent::USLAIAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLAIAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeData();
}

void USLAIAttributeComponent::InitializeData()
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

	ESLChapterType ChapterType = ESLChapterType::EC_None;
	if (const USLLevelTransferSubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>())
	{
		ChapterType = LevelSubsystem->GetCurrentChapter();
	}
	
	if (ChapterType == ESLChapterType::EC_None)
	{
		UE_LOG(LogTemp, Error, TEXT("UAIAttributeComponent: AIChapterType이 'None'이거나 유효한 챕터 정보를 가져오지 못했습니다. (%s)"), *GetOwner()->GetName());
	}

	AIChapterType = ConvertToEAIChapterType(ChapterType);
	SetAIStat(AIChapterType, AIUnitType);
}

void USLAIAttributeComponent::SetAIStat(EAIChapterType NewChapterType, EAIUnitType NewUnitType)
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
		AttackRange = 450.f;
		AvailDistance = 800.0f;
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

void USLAIAttributeComponent::ToggleBerserkMode(bool bEnable)
{
	if (bIsBerserkModeActive == bEnable)
	{
		return;
	}

	bIsBerserkModeActive = bEnable;
	ASLMonsterAICharacter* OwnerCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;
	
	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();

	if (!IsValid(MoveComp) || !IsValid(MeshComp)) return;

	if (bEnable)
	{
		OriginalMaxWalkSpeed = MoveComp->MaxWalkSpeed;

		MeshComp->GlobalAnimRateScale = 1.8f;
		MoveComp->MaxWalkSpeed = 750.0f;
	}
	else
	{
		MeshComp->GlobalAnimRateScale = 1.5f;
		MoveComp->MaxWalkSpeed = OriginalMaxWalkSpeed;
	}
}

void USLAIAttributeComponent::ApplyRandomStatsToOwner(const FAIUnitStatsData& StatsData)
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
			OriginalMaxWalkSpeed = RandomSpeed;
			UE_LOG(LogTemp, Log, TEXT("  - 이동 속도 (랜덤): %.2f (%.2f ~ %.2f)"), RandomSpeed, StatsData.MovementSpeedMin, StatsData.MovementSpeedMax);
		}
	}
}

ESLChapterType USLAIAttributeComponent::ConvertToESLChapterType(const EAIChapterType AIType)
{
	switch (AIType)
	{
	case EAIChapterType::Chapter1:
		return ESLChapterType::EC_Chapter1;
	case EAIChapterType::Chapter2:
		return ESLChapterType::EC_Chapter2;
	case EAIChapterType::Chapter3:
		return ESLChapterType::EC_Chapter3;
	case EAIChapterType::Chapter4:
		return ESLChapterType::EC_Chapter4;
	case EAIChapterType::None:
	default:
		return ESLChapterType::EC_None;
	}
}


EAIChapterType USLAIAttributeComponent::ConvertToEAIChapterType(const ESLChapterType SLType)
{
	switch (SLType)
	{
	case ESLChapterType::EC_Chapter0:
	case ESLChapterType::EC_Chapter1:
		return EAIChapterType::Chapter1;
	case ESLChapterType::EC_Chapter2:
		return EAIChapterType::Chapter2;
	case ESLChapterType::EC_Chapter3:
		return EAIChapterType::Chapter3;
	case ESLChapterType::EC_Chapter4:
		return EAIChapterType::Chapter4;
	case ESLChapterType::EC_None:
	default:
		return EAIChapterType::None;
	}
}

