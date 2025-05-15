#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TagQueryDataAsset.generated.h"

UCLASS()
class STILLLOADING_API UTagQueryDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tag Query")
	FGameplayTagQuery TagQuery;
};
