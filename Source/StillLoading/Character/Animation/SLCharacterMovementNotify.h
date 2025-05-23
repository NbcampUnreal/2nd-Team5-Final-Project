#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLCharacterMovementNotify.generated.h"

UENUM(BlueprintType)
enum class ECharacterMovementAction : uint8
{
	CMA_Launch		UMETA(DisplayName = "Launch"),
};

UCLASS()
class STILLLOADING_API USLCharacterMovementNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	ECharacterMovementAction MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector LaunchVector = FVector(0.f, 0.f, 800.f);

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
