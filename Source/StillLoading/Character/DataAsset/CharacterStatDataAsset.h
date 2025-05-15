#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterStatDataAsset.generated.h"

UCLASS()
class STILLLOADING_API UCharacterStatDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float WalkingSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float MaxSpeed = 500.f;
};
