#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Character/Animation/SLCharacterMovementNotify.h"
#include "SLMonsterMovementNotify.generated.h"

UCLASS()
class STILLLOADING_API USLMonsterMovementNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	ECharacterMovementAction MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float LaunchPower = 500.f;

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