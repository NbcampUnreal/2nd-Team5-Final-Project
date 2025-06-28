#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLMonsterHitRecoveryNotify.generated.h"

UCLASS()
class STILLLOADING_API USLMonsterHitRecoveryNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
