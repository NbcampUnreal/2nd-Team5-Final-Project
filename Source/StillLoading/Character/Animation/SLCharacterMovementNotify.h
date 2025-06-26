#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLCharacterMovementNotify.generated.h"

UENUM(BlueprintType)
enum class ECharacterMovementAction : uint8
{
	CMA_LaunchUp			UMETA(DisplayName = "Launch Up"),
	CMA_LaunchBack			UMETA(DisplayName = "Launch Back"),
	CMA_LaunchOff			UMETA(DisplayName = "Launch Off"),
	CMA_LaunchActionSword	UMETA(DisplayName = "Launch Action Sword"),
	CMA_LaunchActionShield	UMETA(DisplayName = "Launch Action Shield"),
	CMA_EmpowerSword		UMETA(DisplayName = "EmpowerSword"),
	CMA_DestroySword		UMETA(DisplayName = "DestroySword"),
	CMA_RotateFaceToTarget	UMETA(DisplayName = "Face To Target"),
	CMA_AISpear				UMETA(DisplayName = "Spear"),
	CMA_AIWeaponVisible		UMETA(DisplayName = "Weapon Visible"),
	CMA_EndRotate			UMETA(DisplayName = "End Rotate"),
};

UCLASS()
class STILLLOADING_API USLCharacterMovementNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	ECharacterMovementAction MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float LaunchPower = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float KnockBackDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float KnockBackSpeed = 300.f;

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
