#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "CameraOcclusionHandlerComponent.generated.h"

class UMaterialInterface;

USTRUCT()
struct FActorMaterialSet
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> Materials;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCameraOcclusionHandlerComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UCameraOcclusionHandlerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void OnOcclusionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOcclusionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyTransparencyToMesh(UStaticMeshComponent* Mesh);
	void RestoreMeshMaterial(UStaticMeshComponent* Mesh);

	UPROPERTY(EditAnywhere, Category = "Occlusion")
	TObjectPtr<UMaterialInterface> TransparentMaterial;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugBox = true;

	UPROPERTY()
	TMap<TWeakObjectPtr<UStaticMeshComponent>, FActorMaterialSet> OriginalComponentMaterials;
};