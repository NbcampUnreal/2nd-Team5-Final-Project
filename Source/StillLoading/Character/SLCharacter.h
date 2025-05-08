#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLCharacter.generated.h"

UCLASS()
class STILLLOADING_API ASLCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* CameraComponent;
};
