#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLMonsterAnimNotify.generated.h"

enum class EAttackAnimType : uint8;

UCLASS()
class STILLLOADING_API USLMonsterAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Notify")
	EAttackAnimType MonsterMontageStage;
	
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
