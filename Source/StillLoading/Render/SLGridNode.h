// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "SLGridNode.generated.h"


class UCapsuleComponent;
class ASLGridVolume;
enum class EGridDirection : uint8;
class UBoxComponent;
class USLGridNode;

UCLASS()
class STILLLOADING_API USLGridNodePosition : public UBillboardComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TObjectPtr<USLGridNode> OwnerNode;
};

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
	TObjectPtr<USLGridNodePosition> SpawnPosition;
    
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bUseSpawnPosition = false;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TSoftObjectPtr<ASLGridVolume> ConnectedVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridDirection NodeDirection;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	static void SwitchToVolumeCamera(APlayerController* PlayerController, ASLGridVolume* TargetVolume);
	
protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	UFUNCTION()
	void OnTriggeredNode(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AddTriggerDelegate();
	FVector GetDirectionVector(EGridDirection Direction) const;
	float CalculateTriggerThickness(UBoxComponent* InTriggerVolume, EGridDirection Direction) const;
	float CalculateSafeDistance(const UCapsuleComponent* CharacterCapsule, float TriggerThickness, EGridDirection Direction) const;
	FVector CalculateEdgePosition(const ASLGridVolume* TargetVolume, EGridDirection Direction, float SafeDistance, float ZOffset = 0.0f) const;
	FVector CalculateSpawnBasedTargetLocation(const USLGridNode* TargetNode, const ASLGridVolume* TargetVolume, float SafeDistance, ACharacter* Character) const;
	bool IsSpawnLocationInside(const FVector& SpawnLocation, const FVector& EdgePosition, EGridDirection Direction) const;
	FVector CalculateRelativeOffsetTargetLocation(const ACharacter* Character, const ASLGridVolume* CurrentVolume, const ASLGridVolume* TargetVolume, float SafeDistance) const;
	FVector CalculateRelativeOffset(const FVector& CharacterLocation, const FVector& VolumeLocation, EGridDirection Direction) const;
	void ClampLocationToBounds(FVector& Location, const ASLGridVolume* TargetVolume, float SafeDistance) const;
	bool ValidateRequiredComponents(ACharacter* Character, APlayerController*& OutPlayerController, UCapsuleComponent*& OutCapsuleComponent, ASLGridVolume*& OutTargetVolume, USLGridNode*& OutTargetNode) const;

	const float SafeDistanceBuffer = 10.0f;
	const float TriggerExtentMultiplier = 2.0f;
};
