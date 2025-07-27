#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Components/ActorComponent.h"
#include "SLAITokenSystemComponent.generated.h"

class ASLBattleManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAITokenSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAITokenSystemComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 현재 보유 중인 토큰 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Token")
	TMap<ETokenType, bool> HasTokenMap;

	// 토큰 요청 함수
	UFUNCTION(BlueprintCallable, Category = "AI Token")
	bool RequestToken(ETokenType TokenType);

	// 토큰 반납 함수
	UFUNCTION(BlueprintCallable, Category = "AI Token")
	void ReturnToken(ETokenType TokenType);

	// 특정 토큰을 보유하고 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "AI Token")
	bool HasToken(ETokenType TokenType) const;
};
