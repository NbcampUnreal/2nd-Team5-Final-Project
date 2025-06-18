#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLAnimNotify.generated.h"

UENUM(BlueprintType)
enum class ECharacterMontageState : uint8
{
	// 기본 상태
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Cinematic UMETA(DisplayName = "Cinematic"),
	ECS_Moving UMETA(DisplayName = "Moving"),
	ECS_Jumping UMETA(DisplayName = "Jumping"),
	ECS_Falling UMETA(DisplayName = "Falling"),
	ECS_Dodging UMETA(DisplayName = "Dodging"),
	ECS_Dead UMETA(DisplayName = "Dead"),
	ECS_InputLocked UMETA(DisplayName = "Input Locked"),
	ECS_ETC UMETA(DisplayName = "ETC"),

	// 피격 모션
	ECS_Hit_Weak UMETA(DisplayName = "Hit Weak"),
	ECS_Hit_Medium UMETA(DisplayName = "Hit Medium"),
	ECS_Hit_Area UMETA(DisplayName = "Hit Area"),
	ECS_Hit_Airborne UMETA(DisplayName = "Hit Airborne"),
	ECS_Hit_Groggy UMETA(DisplayName = "Hit Groggy"),
	ECS_Hit_Knockback UMETA(DisplayName = "Hit Knockback"),
	ECS_Hit_Falling UMETA(DisplayName = "Hit Falling"),

	// 공격 시작 모션
	ECS_Attack_Begin UMETA(DisplayName = "Attack Begin"),
	ECS_Attack_BeginAir UMETA(DisplayName = "Attack Begin Air"),

	// 공격 모션
	ECS_Attack_Basic1 UMETA(DisplayName = "Attack Basic 1"),
	ECS_Attack_Basic2 UMETA(DisplayName = "Attack Basic 2"),
	ECS_Attack_Basic3 UMETA(DisplayName = "Attack Basic 3"),
	ECS_Attack_Special1 UMETA(DisplayName = "Attack Special 1"),
	ECS_Attack_Special2 UMETA(DisplayName = "Attack Special 2"),
	ECS_Attack_Special3 UMETA(DisplayName = "Attack Special 3"),
	ECS_Attack_Air1 UMETA(DisplayName = "Attack Air 1"),
	ECS_Attack_Air2 UMETA(DisplayName = "Attack Air 2"),
	ECS_Attack_Air3 UMETA(DisplayName = "Attack Air 3"),
	ECS_Attack_ExecutionA UMETA(DisplayName = "Attack Execution A"),
	ECS_Attack_ExecutionB UMETA(DisplayName = "Attack Execution B"),
	ECS_Attack_ExecutionC UMETA(DisplayName = "Attack Execution C"),
	ECS_Attack_Airborne UMETA(DisplayName = "Attack Airborne"),
	ECS_Attack_Airup UMETA(DisplayName = "Attack Airup"),
	ECS_Attack_Airdown UMETA(DisplayName = "Attack Airdown"),
	ECS_Attack_BlastSword UMETA(DisplayName = "Attack BlastSword"),
	ECS_Attack_BlastShield UMETA(DisplayName = "Attack BlastShield"),
	ECS_Attack_SpawnSword UMETA(DisplayName = "Attack SpawnSword"),

	// 버트 모션
	ECS_Buff UMETA(DisplayName = "Buff"),
	ECS_BuffAir UMETA(DisplayName = "Buff Air"),

	// 방어 모션
	ECS_Defense_Block UMETA(DisplayName = "Defense Block"),
	ECS_Defense_Block_Break UMETA(DisplayName = "Defense Block Break"),
	ECS_Defense_Parry UMETA(DisplayName = "Defense Parry"),

	// 차징 모션
	ECS_Charge_Basic UMETA(DisplayName = "Basic Charge"),
	ECS_Charge_Special UMETA(DisplayName = "Special Charge"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Sword     UMETA(DisplayName = "Sword"),
	IT_Shield    UMETA(DisplayName = "Shield")
};

UCLASS()
class STILLLOADING_API USLAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Notify")
	FString CustomName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Notify")
	ECharacterMontageState CharacterMontageStage;
	
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;
};
