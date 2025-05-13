#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLItem.generated.h"

UCLASS()
class STILLLOADING_API ASLItem : public AActor
{
	GENERATED_BODY()

public:
	ASLItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;
};
