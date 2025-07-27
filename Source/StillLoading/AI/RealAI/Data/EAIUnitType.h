#pragma once

#include "CoreMinimal.h"
#include "EAIUnitType.generated.h"

UENUM(BlueprintType)
enum class EAIUnitType : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Special UMETA(DisplayName = "Special"),
	Ranger UMETA(DisplayName = "Ranger"),
};

UENUM(BlueprintType)
enum class EAIChapterType : uint8
{
	None UMETA(DisplayName = "None"),
	Chapter1 UMETA(DisplayName = "Chapter1"),
	Chapter2 UMETA(DisplayName = "Chapter2"),
	Chapter3 UMETA(DisplayName = "Chapter3"),
	Chapter4 UMETA(DisplayName = "Chapter4"),
};
