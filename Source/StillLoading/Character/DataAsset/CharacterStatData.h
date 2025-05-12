#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterStatData.generated.h"

UCLASS()
class STILLLOADING_API UCharacterStatData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float WalkingSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float MaxSpeed = 500.f;
};
