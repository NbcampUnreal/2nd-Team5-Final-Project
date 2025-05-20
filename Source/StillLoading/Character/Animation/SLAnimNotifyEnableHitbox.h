#pragma once

#include "CoreMinimal.h"
#include "SLAnimNotify.h"
#include "SLAnimNotifyEnableHitbox.generated.h"

UCLASS()
class STILLLOADING_API USLAnimNotifyEnableHitbox : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Notify")
	EItemType TargetItemType;
};
