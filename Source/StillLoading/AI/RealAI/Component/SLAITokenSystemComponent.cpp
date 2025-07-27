#include "SLAITokenSystemComponent.h"

#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"

USLAITokenSystemComponent::USLAITokenSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void USLAITokenSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	//ASLMonsterAICharacterBase AICharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
	for (uint8 i = 0; i < static_cast<uint8>(ETokenType::None) + 1; ++i)
	{
		HasTokenMap.Add(static_cast<ETokenType>(i), false);
	}
}

bool USLAITokenSystemComponent::RequestToken(ETokenType TokenType)
{
	if (HasTokenMap.Contains(TokenType) && HasTokenMap[TokenType])
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: 이미 %s 토큰을 가지고 있습니다."), *GetOwner()->GetName(), *UEnum::GetValueAsString(TokenType));
		return true;
	}

	ASLMonsterAICharacter* AICharacter = Cast<ASLMonsterAICharacter>(GetOwner());
	if (!AICharacter) return false;

	if (AICharacter->BattleManager)
	{
		if (AICharacter->BattleManager->RequestToken(this, TokenType))
		{
			HasTokenMap.Add(TokenType, true);
			UE_LOG(LogTemp, Log, TEXT("%s: %s 토큰 획득 성공."), *GetOwner()->GetName(), *UEnum::GetValueAsString(TokenType));
			return true;
		}
	}
    
	UE_LOG(LogTemp, Log, TEXT("%s: %s 토큰 획득 실패 (대기 중이거나 거부됨)."), *GetOwner()->GetName(), *UEnum::GetValueAsString(TokenType));
	return false;
}

void USLAITokenSystemComponent::ReturnToken(ETokenType TokenType)
{
	if (HasTokenMap.Contains(TokenType) && HasTokenMap[TokenType])
	{
		ASLMonsterAICharacter* AICharacter = Cast<ASLMonsterAICharacter>(GetOwner());
		if (!AICharacter) return;
		
		if (AICharacter->BattleManager)
		{
			AICharacter->BattleManager->ReturnToken(this, TokenType);
			HasTokenMap.Add(TokenType, false);
			UE_LOG(LogTemp, Log, TEXT("%s: %s 토큰 반납 성공."), *GetOwner()->GetName(), *UEnum::GetValueAsString(TokenType));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s 토큰을 가지고 있지 않아 반납할 수 없습니다."), *GetOwner()->GetName(), *UEnum::GetValueAsString(TokenType));
	}
}

bool USLAITokenSystemComponent::HasToken(const ETokenType TokenType) const
{
	if (HasTokenMap.Contains(TokenType))
	{
		return HasTokenMap[TokenType];
	}
	return false;
}

