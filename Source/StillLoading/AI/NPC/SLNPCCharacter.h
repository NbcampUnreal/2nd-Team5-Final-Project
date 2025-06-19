#pragma once

#include "CoreMinimal.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLNPCCharacter.generated.h"

UCLASS()
class STILLLOADING_API ASLNPCCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLNPCCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType AnimType) override;
	virtual void SetHitState(bool bNewIsHit, float AutoResetTime = 0.5f) override;

	void ApplyKnockback(AActor* DamageCauser, float KnockbackForce);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Knockback", meta = (AllowPrivateAccess = "true"))
	float DefaultKnockbackForce;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Knockback", meta = (AllowPrivateAccess = "true"))
	float KnockbackUpwardForce;

	FTimerHandle HitStateResetTimer;
};