#include "InputBufferComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());
	}

	if (!CachedMovementHandlerComponent)
	{
		CachedMovementHandlerComponent = GetOwner()->FindComponentByClass<UMovementHandlerComponent>();
	}
}

void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessBufferedInputs();
}

void UInputBufferComponent::AddBufferedInput(ESkillType Action)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (InputBuffer.Num() >= MaxInputBufferCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputBufferComponent: Input buffer full."));
		return;
	}

	if (Action == ESkillType::ST_PointMove)
	{
		bWaitingForComboWindow = true;
		PointMoveStartTime = CurrentTime;
	}

	if (Action == ESkillType::ST_Block &&
		(LastExecutedSkill == ESkillType::ST_Airborne ||
			LastExecutedSkill == ESkillType::ST_AirUp ||
			LastExecutedSkill == ESkillType::ST_Airdown ||
			LastExecutedSkill == ESkillType::ST_Dodge))
	{
		return;
	}

	LastInputTime = CurrentTime;
	InputBuffer.Add({Action, CurrentTime});

	UE_LOG(LogTemp, Warning, TEXT("InputBufferComponent: Input [%s] array [%d]."), *UEnum::GetValueAsString(Action),
		   InputBuffer.Num());
}

void UInputBufferComponent::ProcessBufferedInputs()
{
	if (OwnerCharacter->IsInPrimaryState(TAG_Character_HitReaction))
	{
		InputBuffer.Empty();
		return;
	}
	
	const UWorld* World = GetWorld();
	if (!World) return;

	const float CurrentTime = World->GetTimeSeconds();

	// 콤보 리셋 조건
	if (LastInputTime >= 0.0f && (CurrentTime - LastInputTime) > BufferDuration)
	{
		if (UCombatHandlerComponent* CombatComp = GetOwner()->FindComponentByClass<UCombatHandlerComponent>())
		{
			CombatComp->ResetCombo();
			UE_LOG(LogTemp, Warning, TEXT("Combo Reset!"));
		}
		LastInputTime = -1.0f;
	}

	// PointMove 콤보 대기 중일 경우
	if (bWaitingForComboWindow)
	{
		if ((CurrentTime - PointMoveStartTime) >= PointMoveExpireTime)
		{
			if (!TryConsumeComboInput())
			{
				UE_LOG(LogTemp, Warning, TEXT("PointMove 콤보 실패 → 전체 입력 폐기"));
				InputBuffer.Empty();
			}
			bWaitingForComboWindow = false;
			PointMoveStartTime = -1.f;
		}
		return;
	}

	// 일반 콤보 처리
	if (TryConsumeComboInput())
		return;

	// 일반 단일 입력 처리 (PointMove 대기 아님)
	if (!InputBuffer.IsEmpty())
	{
		const ESkillType NextInput = InputBuffer[0].Action;

		if (CanConsumeInput(NextInput))
		{
			ExecuteInput(NextInput);
			InputBuffer.RemoveAt(0);
		}
	}
}

TArray<FBufferedInput> GetRecentInputs(const TArray<FBufferedInput>& Source, int32 Count)
{
	TArray<FBufferedInput> Result;

	for (int32 i = 0; i < Count && i < Source.Num(); ++i)
	{
		Result.Add(Source[i]);
	}

	return Result;
}

bool UInputBufferComponent::TryConsumeComboInput()
{
	if (!ComboDataTable || InputBuffer.IsEmpty()) return false;

	for (const auto& RowName : ComboDataTable->GetRowNames())
	{
		const FInputComboRow* ComboRow = ComboDataTable->FindRow<FInputComboRow>(RowName, TEXT("ComboCheck"));
		if (!ComboRow || ComboRow->InputSequence.Num() > InputBuffer.Num()) continue;

		// 최근 입력 N개 추출
		const TArray<FBufferedInput> RecentInputs = GetRecentInputs(InputBuffer, ComboRow->InputSequence.Num());

		// 순서 비교
		bool bMatch = true;
		for (int i = 0; i < ComboRow->InputSequence.Num(); ++i)
		{
			if (RecentInputs[i].Action != ComboRow->InputSequence[i])
			{
				bMatch = false;
				break;
			}
		}

		// 시간 조건
		if (bMatch)
		{
			// 테그 확인
			if (const bool bTagValid = ComboRow->RequiredTag.IsValid())
			{
				if (!OwnerCharacter->IsInPrimaryState(ComboRow->RequiredTag)) continue;
			}

			float StartTime = RecentInputs[0].Timestamp;
			float EndTime = RecentInputs.Last().Timestamp;
			if ((EndTime - StartTime) <= ComboRow->MaxTotalDuration)
			{
				if (CanConsumeInput(ComboRow->ResultSkill))
				{
					ExecuteInput(ComboRow->ResultSkill);
					InputBuffer.RemoveAt(0, ComboRow->InputSequence.Num());

					// 콤보 실행 로그 추가
					FString ComboString;
					for (auto Skill : ComboRow->InputSequence)
					{
						ComboString += UEnum::GetValueAsString(Skill) + TEXT(" ");
					}
					
					FString TagString = ComboRow->RequiredTag.IsValid()
						                    ? ComboRow->RequiredTag.ToString()
						                    : TEXT("None");

					UE_LOG(LogTemp, Log, TEXT("[Combo Matched] %s→ %s (%.2f초, RequiredTag: %s)"),
					       *ComboString,
					       *UEnum::GetValueAsString(ComboRow->ResultSkill),
					       EndTime - StartTime,
					       *TagString
					);

					return true;
				}
			}
		}
	}

	return false;
}

void UInputBufferComponent::ClearBuffer()
{
	InputBuffer.Empty();
	SetComponentTickEnabled(false);
	LastExecutedSkill = ESkillType::ST_None;
}

void UInputBufferComponent::OnIMCActionStarted(EInputActionType ActionType)
{
	ESkillType DecidedSkill = ESkillType::ST_None;

	switch (ActionType)
	{
	case EInputActionType::EIAT_Attack:
		DecidedSkill = ESkillType::ST_Attack;
		break;
	case EInputActionType::EIAT_Block:
		DecidedSkill = ESkillType::ST_Block;
		break;
	case EInputActionType::EIAT_PointMove:
		DecidedSkill = ESkillType::ST_PointMove;
		break;

	default: break;
	}

	if (DecidedSkill != ESkillType::ST_None)
	{
		AddBufferedInput(DecidedSkill);
	}
}

bool UInputBufferComponent::CanConsumeInput(ESkillType NextInput) const
{
	//TODO::동작 조건 정의 필요
	// 공격 연속 3회 -> 공격1 공격2 공격3
	// 공격1 방어 공격1

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AttackBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Attack Blocked"));
		return false;
	}

	return true;
}

void UInputBufferComponent::ExecuteInput(ESkillType Action)
{
	LastExecutedSkill = Action;

	//if (LastExecutedSkill == ESkillType::ST_AirUp && Action == ESkillType::ST_AirUp) return;
	
	if (AActor* Owner = GetOwner())
	{
		if (!CachedMovementHandlerComponent)
		{
			CachedMovementHandlerComponent = GetOwner()->FindComponentByClass<UMovementHandlerComponent>();
		}

		CachedMovementHandlerComponent->HandleBufferedInput(Action);
	}
}
