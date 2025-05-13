#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationMontageComponent.generated.h"

class UBattleComponent;
class ASLCharacter;
class UMontageDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UAnimationMontageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimationMontageComponent();

	UFUNCTION()
	void PlayMontage(UAnimMontage* Montage, FName Section);
	UFUNCTION()
	void PlayAttackMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayDodgeMontage(FName Section = NAME_None);
	UFUNCTION()
	void PlayHitReactMontage();
	UFUNCTION()
	void PlayDeathMontage();
	UFUNCTION()
	bool IsAttackMontagePlaying() const;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages")
	TObjectPtr<UMontageDataAsset> MontageData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimInstance> AnimInstance;

private:
	UFUNCTION()
	void GetAnimInstance();
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	bool IsMontagePlayingHelper(const UAnimMontage* Montage) const;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
};
