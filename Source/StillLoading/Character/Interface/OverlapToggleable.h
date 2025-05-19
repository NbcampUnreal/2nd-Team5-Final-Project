#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OverlapToggleable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UOverlapToggleable : public UInterface
{
	GENERATED_BODY()
};

class IOverlapToggleable
{
	GENERATED_BODY()

public:
	virtual void EnableOverlap() = 0;
	virtual void DisableOverlap() = 0;
};
