#include "SlowMotionHelper.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

TArray<TObjectPtr<USlowMotionRequest>> USlowMotionHelper::SlowMotionQueue;
bool USlowMotionHelper::bIsProcessing = false;

void USlowMotionHelper::ApplyGlobalSlowMotion(UObject* WorldContextObject, float TimeScale, float Duration)
{
	if (!WorldContextObject) return;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);

	FTimerHandle TimerHandle;	
	World->GetTimerManager().SetTimer(TimerHandle, [World]() {
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}, Duration, false);
}

void USlowMotionHelper::ApplyActorSlowMotion(AActor* TargetActor, float TimeScale, float Duration)
{
	if (!TargetActor) return;
	TargetActor->CustomTimeDilation = TimeScale;

	TWeakObjectPtr<AActor> WeakTargetActor = TargetActor;
	
	FTimerHandle TimerHandle;
	TargetActor->GetWorldTimerManager().SetTimer(TimerHandle, [WeakTargetActor]() {
		if (WeakTargetActor.IsValid())
		{
			WeakTargetActor->CustomTimeDilation = 1.0f;
		}
	}, Duration, false);
}

void USlowMotionHelper::ApplySelectiveSlowMotion(AActor* SelfActor, float TimeScale, float Duration)
{
	if (!SelfActor || !SelfActor->GetWorld()) return;
    
	UWorld* World = SelfActor->GetWorld();

	UGameplayStatics::SetGlobalTimeDilation(World, TimeScale);

	SelfActor->CustomTimeDilation = 1.0f / TimeScale;

	TWeakObjectPtr<AActor> WeakSelfActor = SelfActor;

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [World, WeakSelfActor]() {
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);

		if (WeakSelfActor.IsValid())
		{
			WeakSelfActor->CustomTimeDilation = 1.0f;
		}
	}, Duration, false);
}

void USlowMotionHelper::ApplySlowMotionToPawns(AActor* SelfActor, float TimeScale, float Duration)
{
	if (!SelfActor || !SelfActor->GetWorld()) return;

	UWorld* World = SelfActor->GetWorld();

	for (TActorIterator<APawn> It(World); It; ++It)
	{
		APawn* Pawn = *It;
		
		if (Pawn && Pawn != SelfActor)
		{
			Pawn->CustomTimeDilation = TimeScale;
		}
	}

	TWeakObjectPtr<AActor> WeakSelfActor = SelfActor;

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [World, WeakSelfActor]() {
		for (TActorIterator<APawn> It(World); It; ++It)
		{
			APawn* Pawn = *It;
			if (Pawn && Pawn != WeakSelfActor.Get())
			{
				Pawn->CustomTimeDilation = 1.0f;
			}
		}
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
	World->GetTimerManager().SetTimer(TimerHandle, [World, PC, OriginalFOV]() {
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
		if (PC && PC->PlayerCameraManager)
			PC->PlayerCameraManager->SetFOV(OriginalFOV);
	}, Duration, false);
}

void USlowMotionHelper::QueueSlowMotionRequest(UObject* Context, AActor* Target, float TimeScale, float Duration, bool bAffectsAll, bool bExcludeSelf)
{
	if (!Context) return;

	UWorld* World = Context->GetWorld();
	if (!World) return;

	USlowMotionRequest* Request = NewObject<USlowMotionRequest>();
	Request->Context = Context;
	Request->Target = Target;
	Request->TimeScale = TimeScale;
	Request->Duration = Duration;
	Request->bAffectsAll = bAffectsAll;
	Request->bExcludeSelf = bExcludeSelf;

	SlowMotionQueue.Add(Request);

	if (!bIsProcessing)
	{
		ProcessNext(World);
	}
}

void USlowMotionHelper::ProcessNext(UWorld* World)
{
	if (!World || bIsProcessing || SlowMotionQueue.Num() == 0)
		return;

	bIsProcessing = true;

	USlowMotionRequest* Request = SlowMotionQueue[0];
	SlowMotionQueue.RemoveAt(0);

	if (!Request)
	{
		bIsProcessing = false;
		ProcessNext(World);
		return;
	}

	if (Request->bAffectsAll)
	{
		ApplyGlobalSlowMotion(Request->Context, Request->TimeScale, Request->Duration);
	}
	else if (Request->bExcludeSelf)
	{
		ApplySelectiveSlowMotion(Cast<AActor>(Request->Target), Request->TimeScale, Request->Duration);
	}
	else if (Request->Target)
	{
		ApplyActorSlowMotion(Request->Target, Request->TimeScale, Request->Duration);
	}

	const float Delay = Request->Duration;

	UE_LOG(LogTemp, Warning, TEXT("Processing SlowMotion: %f sec"), Delay);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [World]() {
		bIsProcessing = false;
		ProcessNext(World);
	}, Delay, false);
}