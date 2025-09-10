#pragma once

#include "CoreMinimal.h"
#include "Controller/SLBaseAIController.h"
#include "SLStableMasterAIController.generated.h"

class UAISenseConfig_Hearing;

UENUM(BlueprintType)
enum class EInvestigateType : uint8
{
	None        UMETA(DisplayName = "None"),
	Sound       UMETA(DisplayName = "Sound"), 
	Seen        UMETA(DisplayName = "Seen")
};

UCLASS()
class STILLLOADING_API ASLStableMasterAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLStableMasterAIController();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
	virtual void OnTargetPerceptionForgotten(AActor* Actor) override;
	virtual void BeginPlay() override;

private:
	// 자극 처리 함수들
	void HandleTargetLost(AActor* Actor);
	void HandleHidingPlayer(AActor* Actor);
	void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);
	void HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);
	
	// 유틸리티 함수들
	void ClearTargetAndSetInvestigation(AActor* Actor, EInvestigateType InvestigateType);
	void SetWatchingState(AActor* Actor, bool bIsWatching);
	void SetInvestigationLocation(const FVector& Location, EInvestigateType InvestigateType);

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAISenseConfig_Hearing> AISenseConfig_Hearing;

	// 상태 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Hiding", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> CurrentlyWatchingPlayer;
};