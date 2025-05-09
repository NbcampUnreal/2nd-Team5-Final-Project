#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLBaseCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;

UCLASS()
class STILLLOADING_API ASLBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASLBaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* DefaultCamera;

	UPROPERTY(EditAnywhere)
	UCameraComponent* BattleCamera;

	UPROPERTY(EditAnywhere)
	UCameraComponent* TopDownCamera;

	UPROPERTY(EditAnywhere)
	UCameraComponent* SideViewCamera;
};
