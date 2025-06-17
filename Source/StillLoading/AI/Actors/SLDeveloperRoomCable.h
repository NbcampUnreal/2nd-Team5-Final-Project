#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Interactable/Object/SLInteractableBreakable.h"
#include "SLDeveloperRoomCable.generated.h"

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

	UPROPERTY(BlueprintAssignable, Category = "Boss Line")
	FSLOnBossLineDestroyed OnBossLineDestroyed;

protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

	UFUNCTION()
	void OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void UpdateLineVisuals();
	void SetLineState(EBossLineState NewState);
	void SetupLineCollisionResponse();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LineMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> LineCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UStaticMesh> LineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> InactiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Settings")
	TObjectPtr<UMaterialInterface> ActiveMaterial;

private:
	EBossLineState CurrentLineState;
	int32 LineIndex;
};