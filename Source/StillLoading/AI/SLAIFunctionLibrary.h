// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Companion/SLCompanionCharacter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLAIFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Utility|GroundDetection", meta = (WorldContext = "WorldContextObject", DisplayName = "Find Ground Location Beneath", ToolTip = "Performs a line trace downwards from a given location to find the first solid ground hit. Returns the hit location or the trace end if no ground is found within MaxTraceDistance."))
	static FVector FindGroundLocation( const UObject* WorldContextObject, FVector StartLocation, float MaxTraceDistance = 10000.0f, bool bDrawDebug = false);
};
