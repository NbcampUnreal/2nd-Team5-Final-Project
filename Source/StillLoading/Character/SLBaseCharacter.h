#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLBaseCharacter.generated.h"

class UCameraManagerComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class STILLLOADING_API ASLBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASLBaseCharacter();

	// Spring Arm & Camera
	UPROPERTY(VisibleAnywhere) 
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere) 
	TObjectPtr<UCameraComponent> SideViewCamera;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraManagerComponent> CameraManager;
};
