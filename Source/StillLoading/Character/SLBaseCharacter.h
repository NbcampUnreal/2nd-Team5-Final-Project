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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Spring Arm & Camera
	UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> DefaultSpringArm;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> DefaultCamera;

	UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> BattleSpringArm;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> BattleCamera;

	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> TopDownCamera;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> SideViewCamera;

	UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraManagerComponent> CameraManager;
};
