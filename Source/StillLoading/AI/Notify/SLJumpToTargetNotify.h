// SLJumpToTargetNotify.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLJumpToTargetNotify.generated.h"

UCLASS()
class STILLLOADING_API USLJumpToTargetNotify : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    USLJumpToTargetNotify();
    
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    
protected:
    // 회전 업데이트 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
    bool bUpdateRotation;
    
    // 점프에 사용할 시간 (0이면 몽타주 남은 시간 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
    float JumpTime;
    
    // 단일 타겟 블랙보드 키 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
    FName TargetBlackboardKey;
    
    // 포물선 높이 배율
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float ArcHeightMultiplier;

private:
    static TMap<TObjectPtr<USkeletalMeshComponent>, FTimerHandle> JumpTimerHandles;
    
    static TMap<TObjectPtr<USkeletalMeshComponent>, TEnumAsByte<ECollisionEnabled::Type>> OriginalCollisionTypes;
    
    static void FinishJump(USkeletalMeshComponent* MeshComp);
};