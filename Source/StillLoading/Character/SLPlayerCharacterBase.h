#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLPlayerCharacterBase.generated.h"

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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
};
