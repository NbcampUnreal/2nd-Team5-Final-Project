#include "SLAIFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


FVector USLAIFunctionLibrary::FindGroundLocation(const UObject* WorldContextObject, FVector StartLocation, float MaxTraceDistance, bool bDrawDebug)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		return StartLocation;
	}

	FVector TraceStart = StartLocation;
	FVector TraceEnd = StartLocation - FVector(0, 0, MaxTraceDistance); 

	FHitResult HitResult;
	
	ETraceTypeQuery GroundTraceChannel = UEngineTypes::ConvertToTraceType(ECC_WorldStatic);
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APawn::StaticClass(), ActorsToIgnore);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		WorldContextObject,
		TraceStart,
		TraceEnd,
		GroundTraceChannel,
		false,
		ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit)
	{
		return HitResult.Location;
	}
	else
	{
		return TraceEnd; 
	}
}
