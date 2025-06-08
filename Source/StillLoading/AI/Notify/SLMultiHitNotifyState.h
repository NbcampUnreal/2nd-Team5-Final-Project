// SLMultiHitNotifyState.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "SLMultiHitNotifyState.generated.h"

class UBattleComponent;
class ASLAIBaseCharacter;

UENUM(BlueprintType)
enum class EHitShapeType : uint8
{
	EHST_Sphere    UMETA(DisplayName = "Sphere"),
	EHST_Capsule   UMETA(DisplayName = "Capsule"),
	EHST_Box       UMETA(DisplayName = "Box")
};

USTRUCT(BlueprintType)
struct FHitShapeData
{
	GENERATED_BODY()

public:
	FHitShapeData();
	
	// 쉐이프 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	EHitShapeType ShapeType;

	// 스피어 반경 (ShapeType이 Sphere일 때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (EditCondition = "ShapeType == EHitShapeType::EHST_Sphere", EditConditionHides))
	float SphereRadius;

	// 캡슐 반경 (ShapeType이 Capsule일 때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (EditCondition = "ShapeType == EHitShapeType::EHST_Capsule", EditConditionHides))
	float CapsuleRadius;

	// 캡슐 높이의 절반 (ShapeType이 Capsule일 때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (EditCondition = "ShapeType == EHitShapeType::EHST_Capsule", EditConditionHides))
	float CapsuleHalfHeight;

	// 박스 익스텐트 (ShapeType이 Box일 때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (EditCondition = "ShapeType == EHitShapeType::EHST_Box", EditConditionHides))
	FVector BoxExtent;

	// 위치 오프셋 (캐릭터 위치 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	FVector LocationOffset;

	// 회전 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	FRotator RotationOffset;

	// 스윕 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	float SweepDistance;
};

UCLASS()
class STILLLOADING_API USLMultiHitNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	USLMultiHitNotifyState();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	// 히트 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiHit")
	float HitInterval;

	// 최대 히트 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiHit")
	int32 MaxHitCount;

	// 공격 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiHit")
	EAttackAnimType AttackType;

	// 히트 쉐이프 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MultiHit")
	FHitShapeData HitShape;

	// 디버그 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug;

	// 디버그 표시 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bShowDebug"))
	float DebugDrawDuration;

private:
	void PerformHit(ASLAIBaseCharacter* Character,UBattleComponent* BattleComp);
	void DoCustomSweep(ASLAIBaseCharacter* Character,UBattleComponent* BattleComp);
	FCollisionShape GetCollisionShape() const;

	// 현재 히트 카운트
	UPROPERTY()
	int32 CurrentHitCount;

	// 다음 히트까지 남은 시간
	UPROPERTY()
	float TimeUntilNextHit;

	// 이미 히트한 액터들 (프레임 내 중복 방지)
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActorsThisFrame;
};