#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SLInputSettingSubsystem.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class STILLLOADING_API USLInputSettingSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MapKey(UInputMappingContext* Context, const UInputAction* Action, const FKey NewKey) const;

	UFUNCTION(BlueprintCallable)
	TObjectPtr<UInputMappingContext> GetOrCreateIMC(FName ContextName);

private:
	UPROPERTY()
	TMap<FName, TObjectPtr<UInputMappingContext>> MappingContexts;
};
