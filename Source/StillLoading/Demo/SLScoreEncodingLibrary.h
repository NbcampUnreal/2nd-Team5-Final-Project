// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLScoreEncodingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLScoreEncodingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility|ScoreEncoding")
	static FString EncodingScoreToBase36(const int32 InTime, const int32 InCoin);

	UFUNCTION(BlueprintCallable, Category = "Utility|ScoreEncoding")
	static TArray<int32> DecodingBase36ToScore(FString Base36);
	
	UFUNCTION(BlueprintCallable, Category = "Utility|ScoreEncoding")
	static void TestEncoding();
	
private:
	static FString ToBase36(uint64 Value);
	static uint64 FromBase36(const FString& Value);
	
	static constexpr uint64 A = 1000003ULL;
	static constexpr uint64 B = 726591ULL;
	static constexpr uint64 M = 2176782336ULL; // 역원 910375531
	static constexpr uint64 MI = 910375531ULL;
	static constexpr int32 TOKEN_LEN = 6;
};
