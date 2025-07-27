#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAIPushComponent.generated.h"

class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAIPushComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLAIPushComponent();

protected:
	virtual void BeginPlay() override;
	
	int32 GetTeamIDFromController(AController* Controller) const;

	UFUNCTION()
	void OnOwnerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Push")
	TObjectPtr<USphereComponent> OverlapDetectionSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Push")
	float PushForce = 1500.0f;
};
