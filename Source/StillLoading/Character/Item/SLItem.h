#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemOverlap, AActor*, OtherActor);

UCLASS()
class STILLLOADING_API ASLItem : public AActor
{
	GENERATED_BODY()

public:
	ASLItem();

	UPROPERTY(BlueprintAssignable)
	FOnItemOverlap OnItemOverlap;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void BindOverlap(UPrimitiveComponent* OverlapComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
};
