#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLSoundNotify.generated.h"

enum class EBattleSoundType : uint8;

UCLASS()
class STILLLOADING_API USLSoundNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound Type")
	EBattleSoundType SoundType;
};
