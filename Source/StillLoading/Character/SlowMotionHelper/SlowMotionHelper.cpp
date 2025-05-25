#include "SlowMotionHelper.h"
#include "Kismet/GameplayStatics.h"


void USlowMotionHelper::ApplyGlobalSlowMotion(UObject* WorldContextObject, float TimeScale, float Duration)
{
	if (!WorldContextObject) return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [World]()
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}, Duration, false);
}

void USlowMotionHelper::ApplyActorSlowMotion(AActor* TargetActor, float TimeScale, float Duration)
{
	if (!TargetActor) return;

	TargetActor->CustomTimeDilation = TimeScale;

	FTimerHandle TimerHandle;
	TargetActor->GetWorldTimerManager().SetTimer(TimerHandle, [TargetActor]()
	{
		if (TargetActor)
			TargetActor->CustomTimeDilation = 1.0f;
	}, Duration, false);
}

void USlowMotionHelper::ApplyZoomWithSlowMotion(UObject* WorldContextObject, float TimeScale, float Duration, float ZoomFOV)
{
	if (!WorldContextObject) return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PC || !PC->PlayerCameraManager) return;

	const float OriginalFOV = PC->PlayerCameraManager->GetFOVAngle();

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);
	PC->PlayerCameraManager->SetFOV(ZoomFOV);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [World, PC, OriginalFOV]()
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->SetFOV(OriginalFOV);
		}
	}, Duration, false);
}