#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/HitResult.h"
#include "SLFootstepNotify.generated.h"

UCLASS()
class STILLLOADING_API USLFootstepNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    USLFootstepNotify();

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;

private:
    bool PerformGroundTrace(USkeletalMeshComponent* MeshComp, FHitResult& OutHitResult);
    void PlayFootstepSound(const FHitResult& HitResult, const FVector& Location);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep Settings", meta = (AllowPrivateAccess = "true"))
    float TraceDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep Settings", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECollisionChannel> TraceChannel;
};