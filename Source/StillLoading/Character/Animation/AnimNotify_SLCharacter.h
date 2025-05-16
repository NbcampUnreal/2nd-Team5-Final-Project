#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SLCharacter.generated.h"

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

	// 피격 모션
	ECS_Hit_Weak UMETA(DisplayName = "Hit Weak"),
	ECS_Hit_Medium UMETA(DisplayName = "Hit Medium"),
	ECS_Hit_Area UMETA(DisplayName = "Hit Area"),
	ECS_Hit_Airborne UMETA(DisplayName = "Hit Airborne"),
	ECS_Hit_Groggy UMETA(DisplayName = "Hit Groggy"),
	ECS_Hit_Knockback UMETA(DisplayName = "Hit Knockback"),

	// 공격 모션
	ECS_Attack_Basic1 UMETA(DisplayName = "Attack Basic 1"),
	ECS_Attack_Basic2 UMETA(DisplayName = "Attack Basic 2"),
	ECS_Attack_Basic3 UMETA(DisplayName = "Attack Basic 3"),
	ECS_Attack_Special1 UMETA(DisplayName = "Attack Special 1"),
	ECS_Attack_Special2 UMETA(DisplayName = "Attack Special 2"),
	ECS_Attack_Special3 UMETA(DisplayName = "Attack Special 3"),
	ECS_Attack_Air1 UMETA(DisplayName = "Attack Air 1"),
	ECS_Attack_Air2 UMETA(DisplayName = "Attack Air 2 - Windmill"),
	ECS_Attack_Airborn1 UMETA(DisplayName = "Attack Airborn 1"),
	ECS_Attack_Finisher1 UMETA(DisplayName = "Attack Finisher 1"),
	ECS_Attack_Finisher2 UMETA(DisplayName = "Attack Finisher 2 - Air"),

	// 방어 모션
	ECS_Defense_Block UMETA(DisplayName = "Defense Block"),
	ECS_Defense_Parry UMETA(DisplayName = "Defense Parry (Shield Strike)"),

	// 차징 모션
	ECS_Charge_Basic UMETA(DisplayName = "Basic Charge"),
	ECS_Charge_Special UMETA(DisplayName = "Special Charge"),
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
