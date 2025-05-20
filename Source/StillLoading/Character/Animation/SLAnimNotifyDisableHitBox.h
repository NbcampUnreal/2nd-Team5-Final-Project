#pragma once

#include "CoreMinimal.h"
#include "SLAnimNotify.h"
#include "SLAnimNotifyDisableHitBox.generated.h"

UCLASS()
class STILLLOADING_API USLAnimNotifyDisableHitBox : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Notify")
	EItemType TargetItemType;
};
