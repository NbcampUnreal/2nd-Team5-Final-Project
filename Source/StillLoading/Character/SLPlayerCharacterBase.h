#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SLPlayerCharacterBase.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UCameraManagerComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ECharacterRenderingType : uint8
{
	CRT_2D UMETA(DisplayName = "2D"),
	CRT_25D UMETA(DisplayName = "2.5D"),
	CRT_3D UMETA(DisplayName = "3D"),
};

UCLASS()
class STILLLOADING_API ASLPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASLPlayerCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ECharacterRenderingType CharacterType;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
