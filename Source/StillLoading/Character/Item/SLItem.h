#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLItem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemOverlapSignature, AActor*, OverlappedActor);

UCLASS()
class STILLLOADING_API ASLItem : public AActor
{
	GENERATED_BODY()

public:
	ASLItem();

	UFUNCTION()
	void StartOrbit(ACharacter* InOwner);

	UFUNCTION()
	void EndOrbit();

	UPROPERTY(BlueprintAssignable, Category = "Overlap")
	FOnItemOverlapSignature OnItemOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OrbitSpeed = 360.f;

	UPROPERTY(EditAnywhere, Category="Orbit")
	float RotationSpeed = 2520.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BindOverlap(UPrimitiveComponent* OverlapComponent);

	UFUNCTION()
	void BeginSweep();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EmpoweredEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> EmpoweredNiagaraEffect;

private:
	UPROPERTY()
	TObjectPtr<USceneComponent> OrbitCenter;
	UPROPERTY()
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY()
	float OrbitAngle = 0.f;
	UPROPERTY()
	bool bIsOrbiting = false;
	UPROPERTY()
	float OrbitRadius = 100.f;
	UPROPERTY()
	float RadiusExpandSpeed = 100.f;
	UPROPERTY()
	FVector LastLocation;
};
