#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interactable/Object/SLInteractableBreakable.h"
#include "SLDeveloperRoomCable.generated.h"

class UNiagaraSystem;
class USoundBase;
class USkeletalMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossLineDestroyed, int32, LineIndex);

UENUM(BlueprintType)
enum class EBossLineState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Active		UMETA(DisplayName = "Active"),
	Destroyed	UMETA(DisplayName = "Destroyed")
};

UCLASS()
class STILLLOADING_API ASLDeveloperRoomCable : public ASLInteractableBreakable
{
	GENERATED_BODY()

public:
	ASLDeveloperRoomCable();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void ActivateLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void DeactivateLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void DestroyLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void SetLineIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	int32 GetLineIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	EBossLineState GetCurrentLineState() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	bool CanBeDestroyed() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void ForceDestroy();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void HideAndDisable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss Line")
	void OnLineDestroyed();

	UPROPERTY(BlueprintAssignable, Category = "Boss Line")
	FSLOnBossLineDestroyed OnBossLineDestroyed;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

	UFUNCTION()
	void OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void UpdateLineVisuals();
	void SetLineState(EBossLineState NewState);
	void SetupLineCollisionResponse();
	void PlayHitFeedback(const FVector& HitLocation);
	void FlashDamagedMaterial();
	void StartShakeEffect();
	void UpdateShakeEffect(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> LineCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> LineSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<USkeletalMesh> CableSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> InactiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> ActiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> DamagedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float MaterialFlashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Effects")
	float ShakeFrequency;

private:
	EBossLineState CurrentLineState;
	int32 LineIndex;
	FTimerHandle MaterialFlashTimerHandle;
	FTimerHandle ShakeTimerHandle;
	FVector OriginalLocation;
	float ShakeTimeElapsed;
	bool bIsShaking;
};