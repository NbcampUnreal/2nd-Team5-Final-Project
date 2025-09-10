#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLSoundEmitterObject.generated.h"

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	EST_Quiet		UMETA(DisplayName = "Quiet"),
	EST_Normal		UMETA(DisplayName = "Normal"), 
	EST_Loud		UMETA(DisplayName = "Loud")
};

UCLASS()
class STILLLOADING_API ASLSoundEmitterObject : public ASLInteractableObjectBase
{
	GENERATED_BODY()

public:
	ASLSoundEmitterObject();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void EmitSound(ESoundType SoundLevel = ESoundType::EST_Normal, APawn* SoundInstigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void EmitSoundWithParams(float Loudness, float Range, APawn* SoundInstigator = nullptr);

protected:
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	ESoundType DefaultSoundLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	bool bEmitSoundOnInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	bool bEmitSoundOnHit;
};