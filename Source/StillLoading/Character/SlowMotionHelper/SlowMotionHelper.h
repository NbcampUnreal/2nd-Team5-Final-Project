#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlowMotionHelper.generated.h"

UCLASS()
class STILLLOADING_API USlowMotionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 전체 슬로우 모션 적용 */
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyGlobalSlowMotion(UObject* WorldContextObject, float TimeScale = 0.1f, float Duration = 0.3f);

	/** 특정 액터에만 슬로우 모션 적용 */
	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyActorSlowMotion(AActor* TargetActor, float TimeScale = 0.1f, float Duration = 0.3f);

	UFUNCTION(BlueprintCallable, Category = "SlowMotion")
	static void ApplyZoomWithSlowMotion(UObject* WorldContextObject, float TimeScale = 0.1f, float Duration = 0.3f, float ZoomFOV = 50.0f);
};
