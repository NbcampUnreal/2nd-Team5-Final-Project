#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLHideableInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class USLHideableInterface : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API ISLHideableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hiding")
	void SetHidingState(bool bIsHiding);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hiding")
	bool IsHiding() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hiding")
	void OnDetectedWhileHiding();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hiding")
	void SetBeingWatched(bool bIsWatched);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Hiding")
	bool IsBeingWatched() const;
};