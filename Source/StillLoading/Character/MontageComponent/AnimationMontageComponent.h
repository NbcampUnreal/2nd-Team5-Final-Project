#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationMontageComponent.generated.h"

class UBattleComponent;
class ASLPlayerCharacter;
class UMontageDataAsset;

UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	Forward,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UAnimationMontageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimationMontageComponent();

	UFUNCTION()
	void PlayMontage(UAnimMontage* Montage, FName Section);
	UFUNCTION()
	void StopMontage(UAnimMontage* Montage, float BlendOutTime);
	UFUNCTION()
	void Play2DAttackMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayAttackMontage(FName Section = NAME_None);
	UFUNCTION(BlueprintCallable)
	void PlayBluePrintableMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlaySkillMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayHitMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayBlockMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayExecutionMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayTrickMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayDodgeMontage(FName Section = NAME_None);
	UFUNCTION()
	void StopAllMontages(float BlendOutTime);
	UFUNCTION()
	void StopActiveMontages(float BlendOutTime);
	UFUNCTION()
	bool IsAttackMontagePlaying() const;

	// AI
	UFUNCTION()
	void PlayAIAttackMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayAIHitMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayAIETCMontage(FName Section = NAME_None);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UMontageDataAsset> MontageData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimInstance> AnimInstance;

private:
	void GetAnimInstance();
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	bool IsMontagePlayingHelper(const UAnimMontage* Montage) const;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
};
