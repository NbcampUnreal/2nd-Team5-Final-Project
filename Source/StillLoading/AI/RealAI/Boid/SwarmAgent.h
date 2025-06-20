#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SwarmAgent.generated.h"

class ATargetPoint;
class UBoidMovementComponent;

UCLASS()
class STILLLOADING_API ASwarmAgent : public ACharacter
{
	GENERATED_BODY()

public:
	ASwarmAgent();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	TObjectPtr<UBoidMovementComponent> BoidMovementComp;
};
