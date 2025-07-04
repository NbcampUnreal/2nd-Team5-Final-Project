#include "InputBufferComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

DEFINE_LOG_CATEGORY(InputBufferComponent);

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
		UE_LOG(InputBufferComponent, Warning, TEXT("InputBufferComponent: Input buffer full."));
		return;
	}

	if (Action == ESkillType::ST_PointMove)
	{
		bWaitingForComboWindow = true;
		PointMoveStartTime = CurrentTime;
	}

	LastInputTime = CurrentTime;
	InputBuffer.Add({Action, CurrentTime});

	UE_LOG(InputBufferComponent, Warning, TEXT("InputBufferComponent: Input [%s] array [%d]."), *UEnum::GetValueAsString(Action),
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
			UE_LOG(InputBufferComponent, Warning, TEXT("Combo Reset!"));
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
				UE_LOG(InputBufferComponent, Warning, TEXT("PointMove 콤보 실패 → 전체 입력 폐기"));
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

	if (!InputBuffer.IsEmpty())
	{
		while (!InputBuffer.IsEmpty())
		{
			const FBufferedInput& FrontInput = InputBuffer[0];
			if ((CurrentTime - FrontInput.Timestamp) > InputExpireTime)
			{
				UE_LOG(InputBufferComponent, Warning, TEXT("입력 [%s] 폐기됨 (%.2f초 경과)"),
					*UEnum::GetValueAsString(FrontInput.Action),
					CurrentTime - FrontInput.Timestamp);

				InputBuffer.RemoveAt(0);
			}
			else
			{
				break;
			}
		}

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

	TArray<FName> SortedRowNames = ComboDataTable->GetRowNames();
	SortedRowNames.Sort([&](const FName& A, const FName& B)
	{
		const auto* RowA = ComboDataTable->FindRow<FInputComboRow>(A, TEXT("Sort"));
		const auto* RowB = ComboDataTable->FindRow<FInputComboRow>(B, TEXT("Sort"));
		return RowA && RowB ? RowB->InputSequence.Num() < RowA->InputSequence.Num() : false;
	});

	for (const auto& RowName : SortedRowNames)
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
			if (const bool bTagValid = ComboRow->RequiredPrimaryTag.IsValid())
			{
				if (!OwnerCharacter->IsInPrimaryState(ComboRow->RequiredPrimaryTag)) continue;
			}

			if (const bool bTagValid = ComboRow->RequiredSecondaryTag.IsValid())
			{
				if (!OwnerCharacter->HasSecondaryState(ComboRow->RequiredSecondaryTag)) continue;
			}

			const float StartTime = RecentInputs[0].Timestamp;
			const float EndTime = RecentInputs.Last().Timestamp;
			
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
					
					FString TagString = ComboRow->RequiredSecondaryTag.IsValid()
						                    ? ComboRow->RequiredSecondaryTag.ToString()
						                    : TEXT("None");

					UE_LOG(InputBufferComponent, Log, TEXT("[Combo Matched] %s→ %s (%.2f초, RequiredSecondaryTag: %s)"),
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

void UInputBufferComponent::ExecuteInput(const ESkillType Action)
{
	if (LastExecutedSkill == ESkillType::ST_AirUp && Action == ESkillType::ST_AirUp) return;
	
	LastExecutedSkill = Action;
	
	if (AActor* Owner = GetOwner())
	{
		if (!CachedMovementHandlerComponent)
		{
			CachedMovementHandlerComponent = GetOwner()->FindComponentByClass<UMovementHandlerComponent>();
		}

		CachedMovementHandlerComponent->HandleBufferedInput(Action);
	}
}
