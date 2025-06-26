#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpearProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class STILLLOADING_API ASpearProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASpearProjectile();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void SetSpeed(float Speed) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SpearMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	TArray<TObjectPtr<AActor>> HitActors;
};
