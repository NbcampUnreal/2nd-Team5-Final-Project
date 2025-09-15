#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/HitResult.h"
#include "SLFootstepNotify.generated.h"

UENUM(BlueprintType)
enum class EFootstepSoundLevel : uint8
{
    Quiet		UMETA(DisplayName = "Quiet"),
    Normal		UMETA(DisplayName = "Normal"), 
    Loud		UMETA(DisplayName = "Loud")
};

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
    void EmitFootstepNoiseEvent(const FVector& Location, USkeletalMeshComponent* MeshComp);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep Settings", meta = (AllowPrivateAccess = "true"))
    float TraceDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep Settings", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECollisionChannel> TraceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings", meta = (AllowPrivateAccess = "true"))
    EFootstepSoundLevel SoundLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings", meta = (AllowPrivateAccess = "true"))
    bool bEmitNoiseEvent;
};