#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLPlayerCharacterBase.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UCameraManagerComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class STILLLOADING_API ASLPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASLPlayerCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
