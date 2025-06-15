#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlowMotionHelper.generated.h"

UCLASS(BlueprintType)
class USlowMotionRequest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UObject* Context = nullptr;

	UPROPERTY()
	AActor* Target = nullptr;

	UPROPERTY()
	float TimeScale = 0.1f;

	UPROPERTY()
	float Duration = 0.3f;

	UPROPERTY()
	bool bAffectsAll = false;

	UPROPERTY()
	bool bExcludeSelf = false;
};

UCLASS()
class STILLLOADING_API USlowMotionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 전체 슬로우
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyGlobalSlowMotion(UObject* WorldContextObject, float TimeScale = 0.1f, float Duration = 0.3f);

	// 타겟 슬로우
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyActorSlowMotion(AActor* TargetActor, float TimeScale = 0.1f, float Duration = 0.3f);

	// 본인 뺴고 슬로우
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplySelectiveSlowMotion(AActor* SelfActor, float TimeScale, float Duration);

	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplySlowMotionToPawns(AActor* SelfActor, float TimeScale, float Duration);
	
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyZoomWithSlowMotion(UObject* WorldContextObject, float TimeScale = 0.1f, float Duration = 0.3f, float ZoomFOV = 60.0f);

	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void QueueSlowMotionRequest(UObject* Context, AActor* Target, float TimeScale, float Duration, bool bAffectsAll, bool bExcludeSelf);

private:
	static TArray<TObjectPtr<USlowMotionRequest>> SlowMotionQueue;
	static bool bIsProcessing;

	static void ProcessNext(UWorld* World);
};
