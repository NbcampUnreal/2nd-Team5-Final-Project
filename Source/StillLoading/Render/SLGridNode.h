// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SLGridNode.generated.h"


class ASLGridVolume;
enum class EGridDirection : uint8;
class UBoxComponent;

UCLASS()
class STILLLOADING_API USLGridNode : public USceneComponent
{
	GENERATED_BODY()

public:
	USLGridNode();

	void UpdateTriggerVolume();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> TriggerVolume;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UBillboardComponent> SpawnPosition;
    
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bUseSpawnPosition = false;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TSoftObjectPtr<ASLGridVolume> ConnectedVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridDirection NodeDirection;

protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	UFUNCTION()
	void OnTriggeredNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void SwitchToVolumeCamera(APlayerController* PlayerController, ASLGridVolume* TargetVolume);
};
