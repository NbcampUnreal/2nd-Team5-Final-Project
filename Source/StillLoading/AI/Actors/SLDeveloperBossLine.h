#pragma once

#include "CoreMinimal.h"
#include "SLDestructibleActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SLDeveloperBossLine.generated.h"

class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossLineDestroyed, int32, LineIndex);

UCLASS()
class STILLLOADING_API ASLDeveloperBossLine : public ASLDestructibleActor
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
	void SetLineIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Boss Line")
	int32 GetLineIndex() const;

	UPROPERTY(BlueprintAssignable, Category = "Boss Line")
	FSLOnBossLineDestroyed OnBossLineDestroyed;

protected:
	UFUNCTION()
	void OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	virtual void UpdateActorVisuals() override;
	virtual void OnActorDestroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LineMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UStaticMesh> LineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> InactiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> ActiveMaterial;
};