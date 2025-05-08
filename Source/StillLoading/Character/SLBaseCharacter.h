#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLBaseCharacter.generated.h"

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
