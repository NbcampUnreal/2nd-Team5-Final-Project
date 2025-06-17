#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SLDeveloperBossLine.generated.h"

class UNiagaraSystem;
class UBattleComponent;

enum class EHitAnimType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossLineDestroyed, int32, LineIndex);

UENUM(BlueprintType)
enum class EBossLineState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Active		UMETA(DisplayName = "Active"),
	Destroyed	UMETA(DisplayName = "Destroyed")
};

UCLASS()
class STILLLOADING_API ASLDeveloperBossLine : public AActor
{
	GENERATED_BODY()

public:
	ASLDeveloperBossLine();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void ActivateLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void DeactivateLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void DestroyLine();

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	bool CanBeDestroyed() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	void SetLineIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	int32 GetLineIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	EBossLineState GetCurrentState() const;

	UPROPERTY(BlueprintAssignable, Category = "Boss Line")
	FSLOnBossLineDestroyed OnBossLineDestroyed;

protected:
	UFUNCTION()
	void OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	void UpdateLineVisuals();
	void PlayDestroyEffects();
	void SetLineState(EBossLineState NewState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LineMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UStaticMesh> LineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> InactiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> ActiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> ActivationSound;

private:
	EBossLineState CurrentState;
	float CurrentHealth;
	int32 LineIndex;
};